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

// $XORP: xorp/devnotes/template.hh,v 1.2 2003/01/16 19:08:48 mjh Exp $

#ifndef __MIBS_XORPEVENTLOOP_HH__
#define __MIBS_XORPEVENTLOOP_HH__

#define XORP_MODULE_NAME "XORP_EVENTS_FOR_MIBS"
#include <set>
#include <map>
#include "libxorp/timeval.hh"
#include "libxorp/eventloop.hh"

class SnmpEventLoop;

/**
 * @short A singleton @ref EventLoop capable of exporting events to the snmp agent 
 *
 * The @ref SnmpEventLoop class provides an EventLoop to all the MIB modules
 * that need to communicate with XORP.  The modules will use the EventLoop to
 * instanciate listeners required for asyncronous communication with XORP.  
 */


class SnmpEventLoop : public EventLoop {
public:
    typedef std::set<int> FdSet;
    typedef std::map<TimeVal,unsigned int> AlarmMap; 
public:
    ~SnmpEventLoop() 
    { 
	DEBUGMSGTL((_log_name, "shared event loop freed...!\n"));
	_clear_pending_alarms(); 
	_clear_monitored_fds();	
	delete _sel;
    }
    
    
    /**
     * @return reference to the @ref SnmpEventLoop used by all XORP MIB
     * modules.  This is the only way to instantiate this class, since
     * all other constructors are private.  Use it like this:
     *
     * SnmpEventLoop& e = SnmpEventLoop::the_instance(); 
     *
     * Now use 'e' anywhere you would use an event loop in Xorp modules, 
     * for instance:
     *
     * XorpTimer xt = e.new_periodic(100, callback);
     *
     * Before returning from your MIB module, you should export the event
     * loop events to the snmp agent.
     *
     * e.export_events();
     *
     * NOTE:  typically you would never need to call e.run(), since the 
     * MIB modules should not block.
     */
    static SnmpEventLoop& the_instance() 
    {
	if (!_sel) {
	    _sel = new SnmpEventLoop;
	    DEBUGMSGTL((_log_name, "new shared event loop...\n"));
	}
        return *_sel;
    }

    /**
     * this function will transfer the timers and file descriptors
     * from the XORP event list onto the snmp agent registry. This
     * way the snmp agent will be able to monitor their activity
     * and invoke the appropriate callback function in one of XORP's
     * MIB modules.
     */
    void	export_events();     

     /**
     * this is the name this class will use to identify itself in the 
     * snmpd log.  Only used for debugging
     */
    static void set_log_name(const char * n) { _log_name = n; }
    static const char *  get_log_name() { return _log_name; }

protected:
    AlarmMap _pending_alarms;
    FdSet _exported_readfds;
    FdSet _exported_writefds;
    FdSet _exported_exceptfds;

private:
    SnmpEventLoop(SnmpEventLoop& o);		    // Not implemented 
    SnmpEventLoop& operator= (SnmpEventLoop& o);    // Not implemented

    SnmpEventLoop() : EventLoop() {}	// Private to prevent instantiation 
    void _export_timers();
    void _export_fds();
    void _clear_pending_alarms ();
    void _clear_monitored_fds ();
    static SnmpEventLoop * _sel;
    static const char * _log_name;

    friend void run_fd_callbacks (int, void *); 
    friend void run_timer_callbacks(u_int, void *); 
};


#endif // __MIBS_XORPEVENTLOOP_HH__
