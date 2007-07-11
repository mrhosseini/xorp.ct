/*
 * Copyright (c) 2001-2007 International Computer Science Institute
 * See LICENSE file for licensing, conditions, and warranties on use.
 *
 * DO NOT EDIT THIS FILE - IT IS PROGRAMMATICALLY GENERATED
 *
 * Generated by 'clnt-gen'.
 */

#ident "$XORP$"

#include "fea_rawlink_xif.hh"

bool
XrlRawLinkV0p1Client::send_send(
	const char*	dst_xrl_target_name,
	const string&	if_name,
	const string&	vif_name,
	const Mac&	src_address,
	const Mac&	dst_address,
	const uint32_t&	ether_type,
	const vector<uint8_t>&	payload,
	const SendCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "raw_link/0.1/send");
    x.args().add("if_name", if_name);
    x.args().add("vif_name", vif_name);
    x.args().add("src_address", src_address);
    x.args().add("dst_address", dst_address);
    x.args().add("ether_type", ether_type);
    x.args().add("payload", payload);
    return _sender->send(x, callback(this, &XrlRawLinkV0p1Client::unmarshall_send, cb));
}


/* Unmarshall send */
void
XrlRawLinkV0p1Client::unmarshall_send(
	const XrlError&	e,
	XrlArgs*	a,
	SendCB		cb
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
XrlRawLinkV0p1Client::send_register_receiver(
	const char*	dst_xrl_target_name,
	const string&	xrl_target_name,
	const string&	if_name,
	const string&	vif_name,
	const uint32_t&	ether_type,
	const string&	filter_program,
	const bool&	enable_multicast_loopback,
	const RegisterReceiverCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "raw_link/0.1/register_receiver");
    x.args().add("xrl_target_name", xrl_target_name);
    x.args().add("if_name", if_name);
    x.args().add("vif_name", vif_name);
    x.args().add("ether_type", ether_type);
    x.args().add("filter_program", filter_program);
    x.args().add("enable_multicast_loopback", enable_multicast_loopback);
    return _sender->send(x, callback(this, &XrlRawLinkV0p1Client::unmarshall_register_receiver, cb));
}


/* Unmarshall register_receiver */
void
XrlRawLinkV0p1Client::unmarshall_register_receiver(
	const XrlError&	e,
	XrlArgs*	a,
	RegisterReceiverCB		cb
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
XrlRawLinkV0p1Client::send_unregister_receiver(
	const char*	dst_xrl_target_name,
	const string&	xrl_target_name,
	const string&	if_name,
	const string&	vif_name,
	const uint32_t&	ether_type,
	const string&	filter_program,
	const UnregisterReceiverCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "raw_link/0.1/unregister_receiver");
    x.args().add("xrl_target_name", xrl_target_name);
    x.args().add("if_name", if_name);
    x.args().add("vif_name", vif_name);
    x.args().add("ether_type", ether_type);
    x.args().add("filter_program", filter_program);
    return _sender->send(x, callback(this, &XrlRawLinkV0p1Client::unmarshall_unregister_receiver, cb));
}


/* Unmarshall unregister_receiver */
void
XrlRawLinkV0p1Client::unmarshall_unregister_receiver(
	const XrlError&	e,
	XrlArgs*	a,
	UnregisterReceiverCB		cb
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
XrlRawLinkV0p1Client::send_join_multicast_group(
	const char*	dst_xrl_target_name,
	const string&	xrl_target_name,
	const string&	if_name,
	const string&	vif_name,
	const uint32_t&	ether_type,
	const string&	filter_program,
	const Mac&	group_address,
	const JoinMulticastGroupCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "raw_link/0.1/join_multicast_group");
    x.args().add("xrl_target_name", xrl_target_name);
    x.args().add("if_name", if_name);
    x.args().add("vif_name", vif_name);
    x.args().add("ether_type", ether_type);
    x.args().add("filter_program", filter_program);
    x.args().add("group_address", group_address);
    return _sender->send(x, callback(this, &XrlRawLinkV0p1Client::unmarshall_join_multicast_group, cb));
}


/* Unmarshall join_multicast_group */
void
XrlRawLinkV0p1Client::unmarshall_join_multicast_group(
	const XrlError&	e,
	XrlArgs*	a,
	JoinMulticastGroupCB		cb
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
XrlRawLinkV0p1Client::send_leave_multicast_group(
	const char*	dst_xrl_target_name,
	const string&	xrl_target_name,
	const string&	if_name,
	const string&	vif_name,
	const uint32_t&	ether_type,
	const string&	filter_program,
	const Mac&	group_address,
	const LeaveMulticastGroupCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "raw_link/0.1/leave_multicast_group");
    x.args().add("xrl_target_name", xrl_target_name);
    x.args().add("if_name", if_name);
    x.args().add("vif_name", vif_name);
    x.args().add("ether_type", ether_type);
    x.args().add("filter_program", filter_program);
    x.args().add("group_address", group_address);
    return _sender->send(x, callback(this, &XrlRawLinkV0p1Client::unmarshall_leave_multicast_group, cb));
}


/* Unmarshall leave_multicast_group */
void
XrlRawLinkV0p1Client::unmarshall_leave_multicast_group(
	const XrlError&	e,
	XrlArgs*	a,
	LeaveMulticastGroupCB		cb
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