// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-

// Copyright (c) 2001-2003 International Computer Science Institute
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software")
// to deal in the Software without restriction, subject to the conditions
// listed in the XORP LICENSE file. These conditions include: you must
// preserve this copyright notice, and you cannot mention the copyright
// holders in advertising related to the Software without their permission.
// The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
// notice is a summary of the XORP LICENSE file; the license in that file is
// legally binding.

#ident "$XORP: xorp/libxipc/finder_ng_client.cc,v 1.7 2003/03/07 02:14:08 hodson Exp $"

#include "finder_module.h"

#include "libxorp/callback.hh"
#include "libxorp/debug.h"

#include "xrl_error.hh"
#include "finder_ng_client.hh"
#include "finder_xif.hh"
#include "finder_tcp_messenger.hh"

static const char* finder = "finder";

static class TraceFinder {
public:
    TraceFinder() {
	_do_trace = !(getenv("FINDERCLIENTTRACE") == 0);
    }
    inline bool on() const { return _do_trace; }
    operator bool() { return _do_trace; }
    inline void set_context(const string& s) { _context = s; }
    inline const string& context() const { return _context; }
protected:
    bool _do_trace;
    string _context;
} finder_tracer;

#define finder_trace(x...) 						      \
do {									      \
    if (finder_tracer.on()) {						      \
	string r = c_format(x);						      \
	XLOG_INFO(c_format("%s\n", r.c_str()).c_str());	      		      \
    }									      \
} while (0)


#define finder_trace_init(x...) 					      \
do {									      \
    if (finder_tracer.on()) finder_tracer.set_context(c_format(x));	      \
} while (0)

#define finder_trace_result(x...)					      \
do {									      \
    if (finder_tracer.on()) {						      \
	string r = c_format(x);						      \
	XLOG_INFO(c_format("%s -> %s\n",				      \
		  finder_tracer.context().c_str(), r.c_str()).c_str());	      \
    }									      \
} while(0)

/**
 * Base class for operations to be executed by FinderNGClient.
 */
class FinderNGClientOp {
public:
    FinderNGClientOp(FinderNGClient& fc) : _fc(fc) {}
    virtual ~FinderNGClientOp() {}

    /*
     * If execute is called with m == 0, then Messenger is down.  Execute
     * should take suitable action.
     */
    virtual void execute(FinderMessengerBase* m) = 0;

    inline FinderNGClient& client() { return _fc; }
    
protected:
    FinderNGClient& _fc;
};

/**
 * Base class for operations FinderNGClient only need execute once,
 * eg resolutions.
 */
class FinderNGClientOneOffOp : public FinderNGClientOp {
public:
    FinderNGClientOneOffOp(FinderNGClient& fc) : FinderNGClientOp(fc) {}

    virtual void force_failure(const XrlError&) = 0;
};

/**
 * Base class for operations FinderNGClient may have to repeat, eg
 * register target, register xrl, etc.  Each repeat operation is associated
 * with an enumerated target.
 */
class FinderNGClientRepeatOp : public FinderNGClientOp {
public:
    FinderNGClientRepeatOp(FinderNGClient& fc, uint32_t target_id)
	: FinderNGClientOp(fc), _tid(target_id) {}

    inline uint32_t target_id() const { return _tid; }

private:
    uint32_t _tid;
};

/**
 * Class that handles resolutions for FinderNGClient, and puts results
 * into FinderNGClient's resolved table and notifies the client.
 */
class FinderNGClientQuery : public FinderNGClientOneOffOp {
public:
    typedef FinderNGClient::QueryCallback QueryCallback;
    typedef FinderNGClient::ResolvedTable ResolvedTable;
    
public:
    FinderNGClientQuery(FinderNGClient&	fc,
			const string&	key,
			ResolvedTable&	rt,
			const QueryCallback& qcb)
	: FinderNGClientOneOffOp(fc), _key(key), _rt(rt), _qcb(qcb)
    {
	finder_trace("Constructing ClientQuery \"%s\"", _key.c_str());
    }

    ~FinderNGClientQuery()
    {
	finder_trace("Destructing ClientQuery \"%s\"", _key.c_str());
    }
    
    void
    execute(FinderMessengerBase* m)
    {
	finder_trace_init("executing ClientQuery \"%s\"", _key.c_str());
	XrlFinderV0p1Client cl(m);
	if (!cl.send_resolve_xrl(
			 finder, _key, 
			 callback(this, &FinderNGClientQuery::query_callback))
		) {
	    _qcb->dispatch(XrlError::SEND_FAILED(), 0);
	    XLOG_ERROR("Failed on send_resolve_xrl");
	    finder_trace_result("failed (send)");
	    client().notify_failed(this);
	    return;
	}
	finder_trace_result("okay");
    }

    void
    query_callback(const XrlError& e, const XrlAtomList* al)
    {
	finder_trace_init("ClientQuery callback \"%s\"", _key.c_str());
	if (e != XrlError::OKAY()) {
	    finder_trace_result("failed on \"%s\" (%s) -> RESOLVE_FAILED",
				_key.c_str(), e.str().c_str());
	    _qcb->dispatch(XrlError::RESOLVE_FAILED(), 0);

	    if (e == XrlError::COMMAND_FAILED()) {
		// Probably something that doesn't resolve, not a catastrophic
		// transport failure
		client().notify_done(this);
	    } else {
		// Probably a catastrophic transport failure
		client().notify_failed(this);
	    }
	    return;
	}

	pair<ResolvedTable::iterator, bool> result = 
	    _rt.insert(std::make_pair(_key, FinderDBEntry(_key)));

	if (result.second == false && result.first == _rt.end()) {
	    // Out of memory (?)
	    XLOG_ERROR("Failed to add entry for %s to resolve table.\n",
		       _key.c_str());
	    _qcb->dispatch(XrlError::FAILED_UNKNOWN(), 0); // :-(
	    finder_trace_result("failed (unknown)");
	    client().notify_failed(this);
	    return;
	}

	// NB if result.second == false and result.first != _rt.end we've
	// made back to back requests for something that is not in cache
	// and both results have come in.  WE take the more recent result
	// as the one to use.

	ResolvedTable::iterator& rt_entry = result.first;
	if (false == rt_entry->second.values().empty()) {
	    rt_entry->second.clear();
	}

	for (size_t i = 0; i < al->size(); i++) {
	    try {
		debug_msg("Adding resolved \"%s\"\n", al->get(i).text().c_str());
		rt_entry->second.values().push_back(al->get(i).text());
	    } catch (const XrlAtom::NoData&) {
		_rt.erase(rt_entry);
		_qcb->dispatch(XrlError::RESOLVE_FAILED(), 0);
		finder_trace_result("failed (corrupt response)");
		client().notify_done(this);
		return;
	    } catch (const XrlAtom::WrongType&) {
		_rt.erase(rt_entry);
		_qcb->dispatch(XrlError::RESOLVE_FAILED(), 0);
		finder_trace_result("failed (corrupt response)");
		client().notify_done(this);
		return;
	    }
	}
	_qcb->dispatch(e, &rt_entry->second);
	finder_trace_result("okay");
	client().notify_done(this);
    }

    void force_failure(const XrlError& e)
    {
	finder_trace("ClientQuery force_failure \"%s\"", _key.c_str());
	_qcb->dispatch(e, 0);
    }
    
protected:
    string	   _key;
    ResolvedTable& _rt;
    QueryCallback  _qcb;
};

/**
 * Class that handles the forwarding of Xrl's targetted at the finder.
 */
class FinderNGForwardedXrl : public FinderNGClientOneOffOp {
public:
    typedef XrlPFSender::SendCallback XrlCallback;
public:
    FinderNGForwardedXrl(FinderNGClient&	fc,
			 const Xrl& 		xrl,
			 const XrlCallback&	xcb)
	: FinderNGClientOneOffOp(fc), _xrl(xrl), _xcb(xcb)
    {
	finder_trace("Constructing ForwardedXrl \"%s\"", _xrl.str().c_str());
    }

    ~FinderNGForwardedXrl()
    {
	finder_trace("Destructing ForwardedXrl \"%s\"", _xrl.str().c_str());
    }
    
    void
    execute(FinderMessengerBase* m)
    {
	finder_trace_init("executing ForwardedXrl \"%s\"", _xrl.str().c_str());
	if (m->send(_xrl,
		    callback(this, &FinderNGForwardedXrl::execute_callback))) {
	    finder_trace_result("okay");
	    return;
	}
	_xcb->dispatch(XrlError::SEND_FAILED(), _xrl, 0);
	XLOG_ERROR("Failed to send forwarded Xrl to Finder.");
	client().notify_failed(this);
	finder_trace_result("failed (send)");
	return;
    }

    void
    execute_callback(const XrlError& e, XrlArgs* args)
    {
	finder_trace_init("ForwardedXrl callback \"%s\"", _xrl.str().c_str());
	_xcb->dispatch(e, _xrl, args);
	client().notify_done(this);
	finder_trace_result("%s", e.str().c_str());
    }

    void force_failure(const XrlError& e)
    {
	finder_trace("ForwardedXrl force_failure \"%s\"", _xrl.str().c_str());
	_xcb->dispatch(e, _xrl, 0);
    }

    
protected:
    Xrl		_xrl;
    XrlCallback	_xcb;
};

/**
 * Class to register client with Finder.
 */
class FinderNGClientRegisterTarget : public FinderNGClientRepeatOp {
public:
    FinderNGClientRegisterTarget(FinderNGClient& fc,
				 uint32_t	 target_id,
				 const string&	 instance_name,
				 const string&	 class_name)
	: FinderNGClientRepeatOp(fc, target_id),
	  _iname(instance_name), _cname(class_name), _cookie("")
    {}

    void execute(FinderMessengerBase* m)
    {
	FinderTcpMessenger *ftm = dynamic_cast<FinderTcpMessenger*>(m);
	XLOG_ASSERT(ftm != 0);
	XrlFinderV0p1Client cl(m);
	if (!cl.send_register_finder_client(finder, _iname, _cname, _cookie,
		callback(this, &FinderNGClientRegisterTarget::reg_callback))) {
	    XLOG_ERROR("Failed on send_register_xrl");
	    client().notify_failed(this);
	} else {
	    debug_msg("Sent register_finder_client(\"%s\", \"%s\")\n",
		      _iname.c_str(), _cname.c_str());
	}
    }

    void reg_callback(const XrlError& e, const string* out_cookie)
    {
	if (e == XrlError::OKAY()) {
	    _cookie = *out_cookie;
	    debug_msg("register_finder_client gave cookie = %s\n",
		      _cookie.c_str());
	    client().notify_done(this);
	    return;
	}

	XLOG_ERROR("Failed to register client named %s of class %s: \"%s\"\n",
		   _iname.c_str(), _cname.c_str(), e.str().c_str());
	client().notify_failed(this);
    }
    
protected:
    string _iname;
    string _cname;
    string _cookie;
};

/**
 * Class to register an Xrl with the Finder.
 */
class FinderNGClientRegisterXrl : public FinderNGClientRepeatOp {
public:
    typedef FinderNGClient::LocalResolvedTable LocalResolvedTable;

public:
    FinderNGClientRegisterXrl(FinderNGClient&	  fc,
			      LocalResolvedTable& lrt,
			      uint32_t	 	  target_id,
			      const string&	  xrl,
			      const string&	  pf_name,
			      const string&	  pf_args)
	: FinderNGClientRepeatOp(fc, target_id), _lrt(lrt),
	  _xrl(xrl), _pf(pf_name), _pf_args(pf_args)
    {}

    void execute(FinderMessengerBase* m)
    {
	XrlFinderV0p1Client cl(m);
	debug_msg("Sending add_xrl(\"%s\", \"%s\", \"%s\")\n",
		  _xrl.c_str(), _pf.c_str(), _pf_args.c_str());
	if (!cl.send_add_xrl(finder, _xrl, _pf, _pf_args,
		callback(this, &FinderNGClientRegisterXrl::reg_callback))) {
	    XLOG_ERROR("Failed on send_add_xrl");
	    client().notify_failed(this);
	}
    }

    void
    reg_callback(const XrlError& e, const string* result)
    {
	if (XrlError::OKAY() == e) {
	    Xrl x(_xrl.c_str());
	    debug_msg("Registering command mapping \"%s\" -> \"%s\"\n",
		      result->c_str(), x.command().c_str());
	    _lrt[*result] = x.command();
	    client().notify_done(this);
	} else {
	   XLOG_ERROR("Failed to register xrl %s: %s\n",
		      _xrl.c_str(), e.str().c_str());
	   client().notify_failed(this);
	}
    }
    
protected:
    LocalResolvedTable& _lrt;
    string _xrl;
    string _pf;
    string _pf_args;
};

class FinderNGClientEnableXrls : public FinderNGClientRepeatOp {
public:
    FinderNGClientEnableXrls(FinderNGClient& fc,
			     uint32_t        target_id,
			     const string&   instance_name,
			     bool	     en)
	: FinderNGClientRepeatOp(fc, target_id), _iname(instance_name), _en(en)
    {}

    void execute(FinderMessengerBase* m)
    {
	FinderTcpMessenger *ftm = dynamic_cast<FinderTcpMessenger*>(m);
	XLOG_ASSERT(ftm != 0);
	XrlFinderV0p1Client cl(m);
	if (!cl.send_set_finder_client_enabled(finder, _iname, _en,
		callback(this, &FinderNGClientEnableXrls::en_callback))) {
	    XLOG_ERROR("Failed on send_set_finder_client_enabled");
	    client().notify_failed(this);
	}
    }
    void en_callback(const XrlError& e)
    {
	if (e == XrlError::OKAY()) {
	    client().notify_done(this);
	    return;
	}
	XLOG_ERROR("Failed to enable client \"%s\": %s\n",
		   _iname.c_str(), e.str().c_str());
	client().notify_failed(this);
    }

protected:
    string	_iname;
    bool	_en;
};

///////////////////////////////////////////////////////////////////////////////
//
// FinderNGClient methods
//

FinderNGClient::FinderNGClient()
    : _messenger(0), _pending_result(false) 
{
}

FinderNGClient::~FinderNGClient()
{
    if (_messenger) {
	delete _messenger;
	_messenger = 0;
    }
}

bool
FinderNGClient::register_xrl_target(const string& instance_name,
				    const string& class_name,
				    uint32_t&	  target_id)
{
    if (instance_name.empty())
	return false;

    for (size_t i = 0; i < _tids.size(); i++) {
	if (_tids[i] == instance_name) {
	    XLOG_WARNING("Attempting to re-register xrl target \"%s\"",
			 instance_name.c_str());
	    target_id = i;
	    return true;
	}
    }

    target_id = _tids.size();
    _tids.push_back(instance_name);

    // We should check whether item exists on queue already.
    Operation op(new FinderNGClientRegisterTarget(*this, target_id,
						  instance_name, class_name));
    _todo_list.push_back(op);
    crank();
    return true;
}

bool
FinderNGClient::register_xrl(uint32_t target_id,
			     const string& xrl,
			     const string& pf_name,
			     const string& pf_args)
{
    if (target_id >= _tids.size() || _tids[target_id].empty())
	return false;

    Operation op(new FinderNGClientRegisterXrl(*this, _lrt, target_id, xrl,
					       pf_name, pf_args));
    _todo_list.push_back(op);
    crank();
    return true;
}

bool
FinderNGClient::enable_xrls(uint32_t target_id)
{
    if (target_id >= _tids.size() || _tids[target_id].empty())
	return false;

    Operation op(new FinderNGClientEnableXrls(*this, target_id,
					      _tids[target_id], true));
    _todo_list.push_back(op);
    crank();
    return true;
}

void
FinderNGClient::query(const string& key,
		      const QueryCallback& qcb)
{
    ResolvedTable::const_iterator i = _rt.find(key);
    if (_rt.end() != i) {
	// Entry exists.
	qcb->dispatch(XrlError::OKAY(), &i->second);
	return;
    }

    Operation op(new FinderNGClientQuery(*this, key, _rt, qcb));
    _todo_list.push_back(op);
    crank();
}

bool
FinderNGClient::query_self(const string& incoming_xrl_cmd,
			   string& local_xrl) const
{
    LocalResolvedTable::const_iterator i = _lrt.find(incoming_xrl_cmd);
    if (_lrt.end() == i)
	return false;
    local_xrl = i->second;
    return true;
}

bool
FinderNGClient::forward_finder_xrl(const Xrl&				xrl,
				   const XrlPFSender::SendCallback& 	scb)
{
    Operation op(new FinderNGForwardedXrl(*this, xrl, scb));
    _todo_list.push_back(op);
    crank();
    return true;
}

FinderMessengerBase*
FinderNGClient::messenger()
{
    return _messenger;
}

void
FinderNGClient::crank()
{
    if (_pending_result)
	return;

    if (0 == _messenger)
	return;

    if (_todo_list.empty())
	return;

    _pending_result = true;
    _todo_list.front()->execute(_messenger);
}

void
FinderNGClient::notify_done(const FinderNGClientOp* op)
{
    // These assertions probably want revising.
    XLOG_ASSERT(_todo_list.empty() == false);
    XLOG_ASSERT(_todo_list.front().get() == op);
    XLOG_ASSERT(_pending_result == true);
    
    // If item is repeatable, ie we'd need to repeat it after a failure
    // put in the done list so we can recover it later.
    if (dynamic_cast<const FinderNGClientRepeatOp*>(op))
	_done_list.push_back(_todo_list.front());
    _todo_list.erase(_todo_list.begin());
    _pending_result = false;
    crank();
}

void
FinderNGClient::notify_failed(const FinderNGClientOp* op)
{
    debug_msg("Client op failed, restarting...\n");

    // These assertions probably want revising.
    XLOG_ASSERT(_todo_list.empty() == false);
    XLOG_ASSERT(_todo_list.front().get() == op);
    XLOG_ASSERT(_pending_result == true);

    if (dynamic_cast<const FinderNGClientRepeatOp*>(op)) {
	_done_list.push_back(_todo_list.front());
    }
    _todo_list.erase(_todo_list.begin());

    // Walk todo list and clear up one off requests.
    OperationQueue::iterator i = _todo_list.begin();
    while (i != _todo_list.end()) {
	OperationQueue::iterator curr = i++;
	FinderNGClientOneOffOp* o = 
	    dynamic_cast<FinderNGClientOneOffOp*>(curr->get());
	if (o) {
	    o->force_failure(XrlError::NO_FINDER());
	}
	_todo_list.erase(curr);
    }
    
    _pending_result = false;

    // trigger restart
    //    delete _messenger;
    //    _messenger = 0;
}

void
FinderNGClient::prepare_for_restart()
{
    // Take all of operations on the done list and put at front of the todo
    // list.
    size_t old_size = _todo_list.size();
    _todo_list.splice(_todo_list.begin(), _done_list);
    XLOG_ASSERT(_todo_list.size() >= old_size);
    
    // Clear resolved table
    _rt.clear();

    // Clear local resolutions
    _lrt.clear();
    
    _pending_result = false;
}

///////////////////////////////////////////////////////////////////////////////
//
// FinderMessengerManager Interface
//

void
FinderNGClient::messenger_birth_event(FinderMessengerBase* m)
{
    debug_msg("messenger %p birth\n", m);    
    XLOG_ASSERT(0 == _messenger);
    prepare_for_restart();
    _messenger = m;
    crank();
}

void
FinderNGClient::messenger_death_event(FinderMessengerBase* m)
{
    debug_msg("messenger %p death\n", m);
    XLOG_ASSERT(m == _messenger);
    _messenger = 0;
}

void
FinderNGClient::messenger_active_event(FinderMessengerBase* m)
{
    // nothing to do - only have one messenger
    debug_msg("messenger %p active\n", m);
    XLOG_ASSERT(m == _messenger);
}

void
FinderNGClient::messenger_inactive_event(FinderMessengerBase* m)
{
    // nothing to do - only have one messenger
    debug_msg("messenger %p inactive\n", m);
    XLOG_ASSERT(m == _messenger);
}

void
FinderNGClient::messenger_stopped_event(FinderMessengerBase* m)
{
    debug_msg("messenger %p stopped (closing connecting)\n", m);
    XLOG_ASSERT(m == _messenger);
    //    delete _messenger;
    //    _messenger = 0;
}

bool
FinderNGClient::manages(const FinderMessengerBase* m) const
{
    return m == _messenger;
}

///////////////////////////////////////////////////////////////////////////////
//
// FinderNGClientXrlCommandInterface
//

void
FinderNGClient::uncache_xrl(const string& xrl)
{
    debug_msg("Request to uncache xrl \"%s\"\n", xrl.c_str());

    ResolvedTable::iterator i = _rt.find(xrl);
    if (_rt.end() != i) {
	debug_msg("Request fulfilled.\n");
	_rt.erase(i);
	return;
    }
    debug_msg("Request not fulfilled - not in cache.\n");
}

void
FinderNGClient::uncache_xrls_from_target(const string& target)
{
    size_t n = 0;
    ResolvedTable::iterator i = _rt.begin();
    while (_rt.end() != i) {
	if (Xrl(i->first.c_str()).target() == target) {
	    _rt.erase(i++);
	    n++;
	} else {
	    ++i;
	}
    }
    debug_msg("Uncached %u Xrls relating to target \"%s\"\n",
	      (uint32_t)n, target.c_str());
}
