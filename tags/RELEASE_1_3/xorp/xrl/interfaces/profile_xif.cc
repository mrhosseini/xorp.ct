/*
 * Copyright (c) 2001-2006 International Computer Science Institute
 * See LICENSE file for licensing, conditions, and warranties on use.
 *
 * DO NOT EDIT THIS FILE - IT IS PROGRAMMATICALLY GENERATED
 *
 * Generated by 'clnt-gen'.
 */

#ident "$XORP: xorp/xrl/interfaces/profile_xif.cc,v 1.5 2006/03/16 00:06:17 pavlin Exp $"

#include "profile_xif.hh"

bool
XrlProfileV0p1Client::send_enable(
	const char*	dst_xrl_target_name,
	const string&	pname,
	const EnableCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "profile/0.1/enable");
    x.args().add("pname", pname);
    return _sender->send(x, callback(this, &XrlProfileV0p1Client::unmarshall_enable, cb));
}


/* Unmarshall enable */
void
XrlProfileV0p1Client::unmarshall_enable(
	const XrlError&	e,
	XrlArgs*	a,
	EnableCB		cb
)
{
    if (e != XrlError::OKAY()) {
	cb->dispatch(e);
	return;
    } else if (a && a->size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != %u)", XORP_UINT_CAST(a->size()), XORP_UINT_CAST(0));
	cb->dispatch(XrlError::BAD_ARGS());
	return;
    }
    cb->dispatch(e);
}

bool
XrlProfileV0p1Client::send_disable(
	const char*	dst_xrl_target_name,
	const string&	pname,
	const DisableCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "profile/0.1/disable");
    x.args().add("pname", pname);
    return _sender->send(x, callback(this, &XrlProfileV0p1Client::unmarshall_disable, cb));
}


/* Unmarshall disable */
void
XrlProfileV0p1Client::unmarshall_disable(
	const XrlError&	e,
	XrlArgs*	a,
	DisableCB		cb
)
{
    if (e != XrlError::OKAY()) {
	cb->dispatch(e);
	return;
    } else if (a && a->size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != %u)", XORP_UINT_CAST(a->size()), XORP_UINT_CAST(0));
	cb->dispatch(XrlError::BAD_ARGS());
	return;
    }
    cb->dispatch(e);
}

bool
XrlProfileV0p1Client::send_get_entries(
	const char*	dst_xrl_target_name,
	const string&	pname,
	const string&	instance_name,
	const GetEntriesCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "profile/0.1/get_entries");
    x.args().add("pname", pname);
    x.args().add("instance_name", instance_name);
    return _sender->send(x, callback(this, &XrlProfileV0p1Client::unmarshall_get_entries, cb));
}


/* Unmarshall get_entries */
void
XrlProfileV0p1Client::unmarshall_get_entries(
	const XrlError&	e,
	XrlArgs*	a,
	GetEntriesCB		cb
)
{
    if (e != XrlError::OKAY()) {
	cb->dispatch(e);
	return;
    } else if (a && a->size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != %u)", XORP_UINT_CAST(a->size()), XORP_UINT_CAST(0));
	cb->dispatch(XrlError::BAD_ARGS());
	return;
    }
    cb->dispatch(e);
}

bool
XrlProfileV0p1Client::send_clear(
	const char*	dst_xrl_target_name,
	const string&	pname,
	const ClearCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "profile/0.1/clear");
    x.args().add("pname", pname);
    return _sender->send(x, callback(this, &XrlProfileV0p1Client::unmarshall_clear, cb));
}


/* Unmarshall clear */
void
XrlProfileV0p1Client::unmarshall_clear(
	const XrlError&	e,
	XrlArgs*	a,
	ClearCB		cb
)
{
    if (e != XrlError::OKAY()) {
	cb->dispatch(e);
	return;
    } else if (a && a->size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != %u)", XORP_UINT_CAST(a->size()), XORP_UINT_CAST(0));
	cb->dispatch(XrlError::BAD_ARGS());
	return;
    }
    cb->dispatch(e);
}

bool
XrlProfileV0p1Client::send_list(
	const char*	dst_xrl_target_name,
	const ListCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "profile/0.1/list");
    return _sender->send(x, callback(this, &XrlProfileV0p1Client::unmarshall_list, cb));
}


/* Unmarshall list */
void
XrlProfileV0p1Client::unmarshall_list(
	const XrlError&	e,
	XrlArgs*	a,
	ListCB		cb
)
{
    if (e != XrlError::OKAY()) {
	cb->dispatch(e, 0);
	return;
    } else if (a && a->size() != 1) {
	XLOG_ERROR("Wrong number of arguments (%u != %u)", XORP_UINT_CAST(a->size()), XORP_UINT_CAST(1));
	cb->dispatch(XrlError::BAD_ARGS(), 0);
	return;
    }
    string info;
    try {
	a->get("info", info);
    } catch (const XrlArgs::XrlAtomNotFound&) {
	XLOG_ERROR("Atom not found");
	cb->dispatch(XrlError::BAD_ARGS(), 0);
	return;
    }
    cb->dispatch(e, &info);
}