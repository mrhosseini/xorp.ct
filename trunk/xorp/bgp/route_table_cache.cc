// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-

// Copyright (c) 2001-2006 International Computer Science Institute
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

#ident "$XORP: xorp/bgp/route_table_cache.cc,v 1.38 2006/09/22 07:12:54 mjh Exp $"

// #define DEBUG_LOGGING
// #define DEBUG_PRINT_FUNCTION_NAME

#include "bgp_module.h"
#include "libxorp/xlog.h"
#include "route_table_cache.hh"
#include "bgp.hh"

template<class A> typename DeleteAllNodes<A>::RouteTables
	DeleteAllNodes<A>::_route_tables;
template<class A> int DeleteAllNodes<A>::_deletions_per_call = 1000;

template<class A>
CacheTable<A>::CacheTable(string table_name,  
			  Safi safi,
			  BGPRouteTable<A> *parent_table,
			  const PeerHandler *peer)
    : BGPRouteTable<A>("CacheTable-" + table_name, safi),
      _peer(peer),
      _unchanged_added(0), _unchanged_deleted(0),
      _changed_added(0), _changed_deleted(0)
{
    this->_parent = parent_table;
    _route_table = new RefTrie<A, const CacheRoute<A> >;
}

template<class A>
CacheTable<A>::~CacheTable()
{
    if (_route_table->begin() != _route_table->end()) {
	XLOG_WARNING("CacheTable trie was not empty on deletion\n");
    }
    delete _route_table;
}

template<class A>
void
CacheTable<A>::flush_cache()
{
    debug_msg("%s\n", this->tablename().c_str());
//     _route_table->delete_all_nodes();
    new DeleteAllNodes<A>(this->_peer, _route_table);
    _route_table = new RefTrie<A, const CacheRoute<A> >;
}

template<class A>
int
CacheTable<A>::add_route(const InternalMessage<A> &rtmsg,
			 BGPRouteTable<A> *caller)
{
    debug_msg("\n         %s\n caller: %s\n rtmsg: %p route: %p\n%s\n",
	      this->tablename().c_str(),
	      caller ? caller->tablename().c_str() : "NULL",
	      &rtmsg,
	      rtmsg.route(),
	      rtmsg.str().c_str());

    XLOG_ASSERT(caller == this->_parent);
    XLOG_ASSERT(this->_next_table != NULL);

    //a cache table is never going to be the last table
    IPNet<A> net = rtmsg.net();

    //check we don't already have this cached
    if (_route_table->lookup_node(net) != _route_table->end()) {
	    crash_dump();
	    XLOG_UNREACHABLE();
    }

    if (rtmsg.changed()==false) {
	log(c_format("add_route (unchanged): %s filters: %p,%p,%p",
		     net.str().c_str(),
		     rtmsg.route()->policyfilter(0).get(),
		     rtmsg.route()->policyfilter(1).get(),
		     rtmsg.route()->policyfilter(2).get()));
	_unchanged_added++;
	return this->_next_table->add_route(rtmsg, (BGPRouteTable<A>*)this);
    } else {
	log(c_format("add_route (changed): %s filters: %p,%p,%p",
		     net.str().c_str(),
		     rtmsg.route()->policyfilter(0).get(),
		     rtmsg.route()->policyfilter(1).get(),
		     rtmsg.route()->policyfilter(2).get()));

	_changed_added++;
	//The route was changed.  

	//It's the responsibility of the recipient of a changed route
	//to store it or free it.  We're the final recipient, so we
	//can rely on rtmsg.route() not to go away.
	const SubnetRoute<A> *msg_route = rtmsg.route();
	//store it locally
	typename RefTrie<A, const CacheRoute<A> >::iterator ti;
	ti = _route_table->insert(msg_route->net(), 
				  CacheRoute<A>(msg_route, rtmsg.genid()));

	assert(ti.payload().route() == msg_route);
	debug_msg("Cache Table: %s\n", this->tablename().c_str());
	debug_msg("Caching route: %p net: %s atts: %p  %s\n", msg_route,
	       msg_route->net().str().c_str(), 
	       (msg_route->attributes()), 
	       msg_route->str().c_str());

	//progogate downstream
	InternalMessage<A> new_rt_msg(ti.payload().route(), 
				      rtmsg.origin_peer(),
				      rtmsg.genid());
	if (rtmsg.push()) new_rt_msg.set_push();
	int result = this->_next_table->add_route(new_rt_msg, 
					    (BGPRouteTable<A>*)this);

	rtmsg.inactivate();

	switch (result) {
	case ADD_USED:
	    ti.payload().route()->set_in_use(true);
#ifdef DUMP_FROM_CACHE
	    //Our cached copy was used, but we need to tell upstream
	    //that their unmodified version was unused.
	    return ADD_UNUSED;
#else
	    //XXX see comment in dump_entire_table - disable dumping
	    //from the cache, so we need to return ADD_USED for now.
	    return ADD_USED;
#endif
	case ADD_UNUSED:
	    ti.payload().route()->set_in_use(false);
	    return ADD_UNUSED;
	default:
	    //In the case of a failure, we don't know what to believe.
	    //To be safe we set in_use to true, although it may reduce
	    //efficiency.
	    msg_route->set_in_use(true);
	    return result;
	}
    }
}

template<class A>
int
CacheTable<A>::replace_route(const InternalMessage<A> &old_rtmsg, 
			     const InternalMessage<A> &new_rtmsg, 
			     BGPRouteTable<A> *caller)
{
    debug_msg("\n         %s\n"
	      "caller: %s\n"
	      "old rtmsg: %p new rtmsg: %p "
	      "old route: %p"
	      "new route: %p"
	      "old: %s\n new: %s\n",
	      this->tablename().c_str(),
	      caller ? caller->tablename().c_str() : "NULL",
	      &old_rtmsg,
	      &new_rtmsg,
	      old_rtmsg.route(),
	      new_rtmsg.route(),
	      old_rtmsg.str().c_str(),
	      new_rtmsg.str().c_str());

    XLOG_ASSERT(caller == this->_parent);
    XLOG_ASSERT(this->_next_table != NULL);

    IPNet<A> net = old_rtmsg.net();
    XLOG_ASSERT(net == new_rtmsg.net());
    log("replace_route: " + net.str());

    SubnetRouteConstRef<A> *old_route_reference = NULL;
    const InternalMessage<A> *old_rtmsg_ptr = &old_rtmsg;
    int result = ADD_USED;

    //do we have the old route cached?
    if (old_rtmsg.changed()==true) {
	log ("old route was changed\n");
	_changed_deleted++;
	typename RefTrie<A, const CacheRoute<A> >::iterator iter;
	iter = _route_table->lookup_node(net);
	if (iter == _route_table->end()) {
	    //We don't flush the cache, so this should not happen
	    crash_dump();
	    XLOG_UNREACHABLE();
	} else {
	    // Preserve the route.  Taking a reference will prevent
	    // the route being immediately deleted when it's erased
	    // from the RefTrie.  Deletion will occur later when the
	    // reference is deleted.
	    const SubnetRoute<A> *old_route = iter.payload().route();
	    old_route_reference = new SubnetRouteConstRef<A>(old_route);

	    old_rtmsg_ptr = new InternalMessage<A>(old_route,
						   old_rtmsg.origin_peer(),
						   iter.payload().genid());

	    //delete it from our cache, 
	    _route_table->erase(old_rtmsg.net());

	    //It's the responsibility of the recipient of a changed
	    //route to store it or free it.
	    //Free the route from the message.
	    old_rtmsg.inactivate();
	}
    } else {
	_unchanged_deleted++;
    }

    //do we need to cache the new route?
    if (new_rtmsg.changed()==false) {
	_unchanged_added++;
	result = this->_next_table->replace_route(*old_rtmsg_ptr, 
					    new_rtmsg, 
					    (BGPRouteTable<A>*)this);
    } else {
	log ("new route was changed\n");
	_changed_added++;
	//Route was changed.

	//It's the responsibility of the recipient of a changed route
	//to store it or free it.  We're the final recipient, so we
	//can rely on rtmsg.route() not to go away.
	const SubnetRoute<A> *new_route = new_rtmsg.route();
	//store it locally
	typename RefTrie<A, const CacheRoute<A> >::iterator ti;
	ti = _route_table->insert(net, CacheRoute<A>(new_route, 
						     new_rtmsg.genid()));
	debug_msg("Caching route2: %p net: %s atts: %p  %s\n", new_route,
	       new_route->net().str().c_str(), 
	       (new_route->attributes()), 
	       new_route->str().c_str());


	//progogate downstream
	InternalMessage<A> new_rtmsg_copy(ti.payload().route(),
					  new_rtmsg.origin_peer(),
					  new_rtmsg.genid());
	if (new_rtmsg.push()) new_rtmsg_copy.set_push();
	result = this->_next_table->replace_route(*old_rtmsg_ptr,
					    new_rtmsg_copy, 
					    (BGPRouteTable<A>*)this);
	new_rtmsg.inactivate();

	switch (result) {
	case ADD_USED:
	    ti.payload().route()->set_in_use(true);
	    break;
	case ADD_UNUSED:
	    ti.payload().route()->set_in_use(false);
	    break;
	default:
	    //In the case of a failure, we don't know what to believe.
	    //To be safe we set in_use to true, although it may reduce
	    //efficiency.
	    ti.payload().route()->set_in_use(true);
	}
    }

    if (old_rtmsg_ptr != &old_rtmsg) {
	delete old_rtmsg_ptr;
	XLOG_ASSERT(old_route_reference != NULL);
	delete old_route_reference;
    }

    return result;
}

template<class A>
int
CacheTable<A>::delete_route(const InternalMessage<A> &rtmsg, 
			    BGPRouteTable<A> *caller)
{
    int result = 0;
    debug_msg("\n         %s\n caller: %s\n rtmsg: %p route: %p\n%s\n",
	      this->tablename().c_str(),
	      caller ? caller->tablename().c_str() : "NULL",
	      &rtmsg,
	      rtmsg.route(),
	      rtmsg.str().c_str());

    XLOG_ASSERT(caller == this->_parent);
    XLOG_ASSERT(this->_next_table != NULL);
    IPNet<A> net = rtmsg.net();
    if (rtmsg.changed()) {
	log(c_format("delete_route (changed): %s filters: %p,%p,%p",
		     net.str().c_str(),
		     rtmsg.route()->policyfilter(0).get(),
		     rtmsg.route()->policyfilter(1).get(),
		     rtmsg.route()->policyfilter(2).get()));
	_changed_deleted++;
    } else {
	log(c_format("delete_route (unchanged): %s filters: %p,%p,%p",
		     net.str().c_str(),
		     rtmsg.route()->policyfilter(0).get(),
		     rtmsg.route()->policyfilter(1).get(),
		     rtmsg.route()->policyfilter(2).get()));
	_unchanged_deleted++;
    }

    //do we already have this cached?
    typename RefTrie<A, const CacheRoute<A> >::iterator iter;
    iter = _route_table->lookup_node(net);
    if (iter != _route_table->end()) {
	const SubnetRoute<A> *existing_route = iter.payload().route();
	uint32_t existing_genid = iter.payload().genid();
	XLOG_ASSERT(rtmsg.genid() == existing_genid);
	XLOG_ASSERT(rtmsg.route()->nexthop() == existing_route->nexthop());
	debug_msg("Found cached route: %s\n", existing_route->str().c_str());

	// Delete it from our cache trie.  The actual deletion will
	// only take place when iter goes out of scope, so
	// existing_route remains valid til then.
	_route_table->erase(iter);

	// Fix the parent route in case it has changed.  This is also
	// important if the parent route has been zeroed somewhere
	// upstream to avoid unwanted side effects.
	const_cast<SubnetRoute<A>*>(existing_route)
	    ->set_parent_route(rtmsg.route()->parent_route());

	InternalMessage<A> old_rt_msg(existing_route,
				      rtmsg.origin_peer(),
				      existing_genid);
	if (rtmsg.push()) old_rt_msg.set_push();

	result = this->_next_table->delete_route(old_rt_msg, 
					   (BGPRouteTable<A>*)this);

	if (rtmsg.changed()) {
	    //It's the responsibility of the recipient of a changed
	    //route to store it or free it.
	    //Free the route from the message.
	    rtmsg.inactivate();
	}
    } else {

	if (rtmsg.changed()) {
	    //
	    // XXX: If we get here, it appears we received delete_route()
	    // operation for a modified route without a matching
	    // add_route() operation for that route.
	    // This could happen, say, if an export policy that
	    // rejects a route has been deleted. In that case the add_route()
	    // with the policy in place would never reach the CacheTable on
	    // the output branch. Deleting the policy however will re-originate
	    // all routes by generating delete_route() followed by add_route().
	    // Such delete_route() without the export policy to reject it
	    // will reach the CacheTable exactly at this part of the code.
	    // Hence we silently return and indicate success.
	    //
	    return result;
	}

	//If we get here, route was not cached and was not modified.
	result = this->_next_table->delete_route(rtmsg, (BGPRouteTable<A>*)this);
    }
    return result;
}

template<class A>
int
CacheTable<A>::push(BGPRouteTable<A> *caller)
{
    XLOG_ASSERT(caller == this->_parent);
    return this->_next_table->push((BGPRouteTable<A>*)this);
}

template<class A>
int 
CacheTable<A>::route_dump(const InternalMessage<A> &rtmsg,
			  BGPRouteTable<A> *caller,
			  const PeerHandler *dump_peer)
{
    XLOG_ASSERT(caller == this->_parent);
    if (rtmsg.changed()) {
	//Check we've got it cached.  Clear the changed bit so we
	//don't confuse anyone downstream.
	IPNet<A> net = rtmsg.route()->net();
	typename RefTrie<A, const CacheRoute<A> >::iterator iter;
	iter = _route_table->lookup_node(net);
	XLOG_ASSERT(iter != _route_table->end());
	XLOG_ASSERT(rtmsg.genid() == iter.payload().genid());

	//It's the responsibility of the recipient of a changed route
	//to store or delete it.  We don't need it anymore (we found
	//the cached copy) so we delete it.
	rtmsg.route()->unref();

	//the message we pass on needs to contain our cached
	//route, because the MED info in it may not be in the original
	//version of the route.
	InternalMessage<A> new_msg(iter.payload().route(), rtmsg.origin_peer(),
				   rtmsg.genid());
	return this->_next_table->route_dump(new_msg, (BGPRouteTable<A>*)this, 
				       dump_peer);
    } else {
	//We must not have this cached
	IPNet<A> net = rtmsg.route()->net();
	XLOG_ASSERT(_route_table->lookup_node(net) == _route_table->end());

	return this->_next_table->route_dump(rtmsg, (BGPRouteTable<A>*)this, 
				       dump_peer);
    }
}

template<class A>
const SubnetRoute<A>*
CacheTable<A>::lookup_route(const IPNet<A> &net,
			    uint32_t& genid) const
{
    //return our cached copy if there is one, otherwise ask our parent
    typename RefTrie<A, const CacheRoute<A> >::iterator iter;
    iter = _route_table->lookup_node(net);
    if (iter != _route_table->end()) {
	genid = iter.payload().genid();
	return iter.payload().route();
    } else {
	return this->_parent->lookup_route(net, genid);
    }
}

template<class A>
void
CacheTable<A>::route_used(const SubnetRoute<A>* rt, bool in_use)
{
    this->_parent->route_used(rt, in_use);
}

template<class A>
string
CacheTable<A>::str() const {
    string s = "CacheTable<A>" + this->tablename();
    return s;
}

template<class A>
bool
CacheTable<A>::get_next_message(BGPRouteTable<A> *next_table)
{
    XLOG_ASSERT(this->_next_table == next_table);

    return this->_parent->get_next_message(this);
}

template<class A>
string
CacheTable<A>::dump_state() const {
    string s;
    s  = "=================================================================\n";
    s += "CacheTable\n";
    s += str() + "\n";
    s += "=================================================================\n";
    s += "Unchanged added: " + c_format("%d\n", _unchanged_added);
    s += "Unchanged deleted: " + c_format("%d\n", _unchanged_deleted);
    s += "Changed added: " + c_format("%d\n", _changed_added); 
    s += "Changed deleted: " + c_format("%d\n", _changed_deleted);
    //    s += "In cache: " + c_format("%d\n", _route_table->size());
    s += _route_table->str();
    s += CrashDumper::dump_state(); 
    return s;
}

template<class A>
EventLoop& 
CacheTable<A>::eventloop() const 
{
    return _peer->eventloop();
}

template class CacheTable<IPv4>;
template class CacheTable<IPv6>;
