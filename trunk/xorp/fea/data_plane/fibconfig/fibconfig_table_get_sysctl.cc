// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-

// Copyright (c) 2001-2007 International Computer Science Institute
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

#ident "$XORP: xorp/fea/forwarding_plane/fibconfig/fibconfig_table_get_sysctl.cc,v 1.3 2007/04/27 01:10:32 pavlin Exp $"

#include "fea_module.h"

#include "libxorp/xorp.h"
#include "libxorp/xlog.h"
#include "libxorp/debug.h"

#ifdef HAVE_SYS_SYSCTL_H
#include <sys/sysctl.h>
#endif

#include "fibconfig.hh"
#include "fibconfig_table_get.hh"


//
// Get the whole table information from the unicast forwarding table.
//
// The mechanism to obtain the information is sysctl(3).
//


FibConfigTableGetSysctl::FibConfigTableGetSysctl(FibConfig& fibconfig)
    : FibConfigTableGet(fibconfig)
{
#ifdef HAVE_SYSCTL_NET_RT_DUMP
    register_fibconfig_primary();
#endif
}

FibConfigTableGetSysctl::~FibConfigTableGetSysctl()
{
    string error_msg;

    if (stop(error_msg) != XORP_OK) {
	XLOG_ERROR("Cannot stop the sysctl(3) mechanism to get "
		   "whole forwarding table from the underlying "
		   "system: %s",
		   error_msg.c_str());
    }
}

int
FibConfigTableGetSysctl::start(string& error_msg)
{
    UNUSED(error_msg);

    if (_is_running)
	return (XORP_OK);

    _is_running = true;

    return (XORP_OK);
}

int
FibConfigTableGetSysctl::stop(string& error_msg)
{
    UNUSED(error_msg);

    if (! _is_running)
	return (XORP_OK);

    _is_running = false;

    return (XORP_OK);
}

bool
FibConfigTableGetSysctl::get_table4(list<Fte4>& fte_list)
{
    list<FteX> ftex_list;
    
    // Get the table
    if (get_table(AF_INET, ftex_list) != true)
	return false;
    
    // Copy the result back to the original list
    list<FteX>::iterator iter;
    for (iter = ftex_list.begin(); iter != ftex_list.end(); ++iter) {
	FteX& ftex = *iter;
	fte_list.push_back(ftex.get_fte4());
    }
    
    return true;
}

bool
FibConfigTableGetSysctl::get_table6(list<Fte6>& fte_list)
{
#ifndef HAVE_IPV6
    UNUSED(fte_list);
    
    return false;
#else
    list<FteX> ftex_list;
    
    // Get the table
    if (get_table(AF_INET6, ftex_list) != true)
	return false;
    
    // Copy the result back to the original list
    list<FteX>::iterator iter;
    for (iter = ftex_list.begin(); iter != ftex_list.end(); ++iter) {
	FteX& ftex = *iter;
	fte_list.push_back(ftex.get_fte6());
    }
    
    return true;
#endif // HAVE_IPV6
}

#ifndef HAVE_SYSCTL_NET_RT_DUMP

bool
FibConfigTableGetSysctl::get_table(int , list<FteX>& )
{
    return false;
}

#else // HAVE_SYSCTL_NET_RT_DUMP

bool
FibConfigTableGetSysctl::get_table(int family, list<FteX>& fte_list)
{
    int mib[6];

    // Check that the family is supported
    switch(family) {
    case AF_INET:
	if (! fibconfig().have_ipv4())
	    return false;
	break;
#ifdef HAVE_IPV6
    case AF_INET6:
	if (! fibconfig().have_ipv6())
	    return false;
	break;
#endif // HAVE_IPV6
    default:
	XLOG_UNREACHABLE();
	break;
    }
    
    mib[0] = CTL_NET;
    mib[1] = AF_ROUTE;
    mib[2] = 0;				// protocol number - always 0
    mib[3] = family;
    mib[4] = NET_RT_DUMP;
    mib[5] = 0;				// no flags
    
    // Get the table size
    size_t sz;
    if (sysctl(mib, sizeof(mib) / sizeof(mib[0]), NULL, &sz, NULL, 0) != 0) {
	XLOG_ERROR("sysctl(NET_RT_DUMP) failed: %s", strerror(errno));
	return false;
    }
    
    //
    // XXX: we need to fetch the data in a loop, because its size
    // may increase between the two sysctl() calls.
    //
    for ( ; ; ) {
	vector<uint8_t> buffer(sz);
	
	// Get the data
	if (sysctl(mib, sizeof(mib) / sizeof(mib[0]), &buffer[0], &sz, NULL, 0)
	    == 0) {
	    // Parse the result
	    return (parse_buffer_rtm(family, fte_list, buffer, FibMsg::GETS));
	}
	
	// Error
	if (errno == ENOMEM) {
	    // Buffer is too small. Try again.
	    continue;
	}
	XLOG_ERROR("sysctl(NET_RT_DUMP) failed: %s", strerror(errno));
	return false;
    }
}

#endif // HAVE_SYSCTL_NET_RT_DUMP