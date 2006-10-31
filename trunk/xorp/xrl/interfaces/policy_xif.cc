/*
 * Copyright (c) 2001-2006 International Computer Science Institute
 * See LICENSE file for licensing, conditions, and warranties on use.
 *
 * DO NOT EDIT THIS FILE - IT IS PROGRAMMATICALLY GENERATED
 *
 * Generated by 'clnt-gen'.
 */

#ident "$XORP: xorp/xrl/interfaces/policy_xif.cc,v 1.12 2006/03/26 22:39:25 pavlin Exp $"

#include "policy_xif.hh"

bool
XrlPolicyV0p1Client::send_create_term(
	const char*	dst_xrl_target_name,
	const string&	policy,
	const string&	order,
	const string&	term,
	const CreateTermCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy/0.1/create_term");
    x.args().add("policy", policy);
    x.args().add("order", order);
    x.args().add("term", term);
    return _sender->send(x, callback(this, &XrlPolicyV0p1Client::unmarshall_create_term, cb));
}


/* Unmarshall create_term */
void
XrlPolicyV0p1Client::unmarshall_create_term(
	const XrlError&	e,
	XrlArgs*	a,
	CreateTermCB		cb
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
XrlPolicyV0p1Client::send_delete_term(
	const char*	dst_xrl_target_name,
	const string&	policy,
	const string&	term,
	const DeleteTermCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy/0.1/delete_term");
    x.args().add("policy", policy);
    x.args().add("term", term);
    return _sender->send(x, callback(this, &XrlPolicyV0p1Client::unmarshall_delete_term, cb));
}


/* Unmarshall delete_term */
void
XrlPolicyV0p1Client::unmarshall_delete_term(
	const XrlError&	e,
	XrlArgs*	a,
	DeleteTermCB		cb
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
XrlPolicyV0p1Client::send_update_term_block(
	const char*	dst_xrl_target_name,
	const string&	policy,
	const string&	term,
	const uint32_t&	block,
	const string&	order,
	const string&	statement,
	const UpdateTermBlockCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy/0.1/update_term_block");
    x.args().add("policy", policy);
    x.args().add("term", term);
    x.args().add("block", block);
    x.args().add("order", order);
    x.args().add("statement", statement);
    return _sender->send(x, callback(this, &XrlPolicyV0p1Client::unmarshall_update_term_block, cb));
}


/* Unmarshall update_term_block */
void
XrlPolicyV0p1Client::unmarshall_update_term_block(
	const XrlError&	e,
	XrlArgs*	a,
	UpdateTermBlockCB		cb
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
XrlPolicyV0p1Client::send_create_policy(
	const char*	dst_xrl_target_name,
	const string&	policy,
	const CreatePolicyCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy/0.1/create_policy");
    x.args().add("policy", policy);
    return _sender->send(x, callback(this, &XrlPolicyV0p1Client::unmarshall_create_policy, cb));
}


/* Unmarshall create_policy */
void
XrlPolicyV0p1Client::unmarshall_create_policy(
	const XrlError&	e,
	XrlArgs*	a,
	CreatePolicyCB		cb
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
XrlPolicyV0p1Client::send_delete_policy(
	const char*	dst_xrl_target_name,
	const string&	policy,
	const DeletePolicyCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy/0.1/delete_policy");
    x.args().add("policy", policy);
    return _sender->send(x, callback(this, &XrlPolicyV0p1Client::unmarshall_delete_policy, cb));
}


/* Unmarshall delete_policy */
void
XrlPolicyV0p1Client::unmarshall_delete_policy(
	const XrlError&	e,
	XrlArgs*	a,
	DeletePolicyCB		cb
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
XrlPolicyV0p1Client::send_create_set(
	const char*	dst_xrl_target_name,
	const string&	set,
	const CreateSetCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy/0.1/create_set");
    x.args().add("set", set);
    return _sender->send(x, callback(this, &XrlPolicyV0p1Client::unmarshall_create_set, cb));
}


/* Unmarshall create_set */
void
XrlPolicyV0p1Client::unmarshall_create_set(
	const XrlError&	e,
	XrlArgs*	a,
	CreateSetCB		cb
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
XrlPolicyV0p1Client::send_update_set(
	const char*	dst_xrl_target_name,
	const string&	type,
	const string&	set,
	const string&	elements,
	const UpdateSetCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy/0.1/update_set");
    x.args().add("type", type);
    x.args().add("set", set);
    x.args().add("elements", elements);
    return _sender->send(x, callback(this, &XrlPolicyV0p1Client::unmarshall_update_set, cb));
}


/* Unmarshall update_set */
void
XrlPolicyV0p1Client::unmarshall_update_set(
	const XrlError&	e,
	XrlArgs*	a,
	UpdateSetCB		cb
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
XrlPolicyV0p1Client::send_delete_set(
	const char*	dst_xrl_target_name,
	const string&	set,
	const DeleteSetCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy/0.1/delete_set");
    x.args().add("set", set);
    return _sender->send(x, callback(this, &XrlPolicyV0p1Client::unmarshall_delete_set, cb));
}


/* Unmarshall delete_set */
void
XrlPolicyV0p1Client::unmarshall_delete_set(
	const XrlError&	e,
	XrlArgs*	a,
	DeleteSetCB		cb
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
XrlPolicyV0p1Client::send_add_to_set(
	const char*	dst_xrl_target_name,
	const string&	type,
	const string&	set,
	const string&	element,
	const AddToSetCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy/0.1/add_to_set");
    x.args().add("type", type);
    x.args().add("set", set);
    x.args().add("element", element);
    return _sender->send(x, callback(this, &XrlPolicyV0p1Client::unmarshall_add_to_set, cb));
}


/* Unmarshall add_to_set */
void
XrlPolicyV0p1Client::unmarshall_add_to_set(
	const XrlError&	e,
	XrlArgs*	a,
	AddToSetCB		cb
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
XrlPolicyV0p1Client::send_delete_from_set(
	const char*	dst_xrl_target_name,
	const string&	type,
	const string&	set,
	const string&	element,
	const DeleteFromSetCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy/0.1/delete_from_set");
    x.args().add("type", type);
    x.args().add("set", set);
    x.args().add("element", element);
    return _sender->send(x, callback(this, &XrlPolicyV0p1Client::unmarshall_delete_from_set, cb));
}


/* Unmarshall delete_from_set */
void
XrlPolicyV0p1Client::unmarshall_delete_from_set(
	const XrlError&	e,
	XrlArgs*	a,
	DeleteFromSetCB		cb
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
XrlPolicyV0p1Client::send_done_global_policy_conf(
	const char*	dst_xrl_target_name,
	const DoneGlobalPolicyConfCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy/0.1/done_global_policy_conf");
    return _sender->send(x, callback(this, &XrlPolicyV0p1Client::unmarshall_done_global_policy_conf, cb));
}


/* Unmarshall done_global_policy_conf */
void
XrlPolicyV0p1Client::unmarshall_done_global_policy_conf(
	const XrlError&	e,
	XrlArgs*	a,
	DoneGlobalPolicyConfCB		cb
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
XrlPolicyV0p1Client::send_import(
	const char*	dst_xrl_target_name,
	const string&	protocol,
	const string&	policies,
	const ImportCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy/0.1/import");
    x.args().add("protocol", protocol);
    x.args().add("policies", policies);
    return _sender->send(x, callback(this, &XrlPolicyV0p1Client::unmarshall_import, cb));
}


/* Unmarshall import */
void
XrlPolicyV0p1Client::unmarshall_import(
	const XrlError&	e,
	XrlArgs*	a,
	ImportCB		cb
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
XrlPolicyV0p1Client::send_export(
	const char*	dst_xrl_target_name,
	const string&	protocol,
	const string&	policies,
	const ExportCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy/0.1/export");
    x.args().add("protocol", protocol);
    x.args().add("policies", policies);
    return _sender->send(x, callback(this, &XrlPolicyV0p1Client::unmarshall_export, cb));
}


/* Unmarshall export */
void
XrlPolicyV0p1Client::unmarshall_export(
	const XrlError&	e,
	XrlArgs*	a,
	ExportCB		cb
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
XrlPolicyV0p1Client::send_add_varmap(
	const char*	dst_xrl_target_name,
	const string&	protocol,
	const string&	variable,
	const string&	type,
	const string&	access,
	const uint32_t&	id,
	const AddVarmapCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy/0.1/add_varmap");
    x.args().add("protocol", protocol);
    x.args().add("variable", variable);
    x.args().add("type", type);
    x.args().add("access", access);
    x.args().add("id", id);
    return _sender->send(x, callback(this, &XrlPolicyV0p1Client::unmarshall_add_varmap, cb));
}


/* Unmarshall add_varmap */
void
XrlPolicyV0p1Client::unmarshall_add_varmap(
	const XrlError&	e,
	XrlArgs*	a,
	AddVarmapCB		cb
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
XrlPolicyV0p1Client::send_dump_state(
	const char*	dst_xrl_target_name,
	const uint32_t&	id,
	const DumpStateCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy/0.1/dump_state");
    x.args().add("id", id);
    return _sender->send(x, callback(this, &XrlPolicyV0p1Client::unmarshall_dump_state, cb));
}


/* Unmarshall dump_state */
void
XrlPolicyV0p1Client::unmarshall_dump_state(
	const XrlError&	e,
	XrlArgs*	a,
	DumpStateCB		cb
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
    string state;
    try {
	a->get("state", state);
    } catch (const XrlArgs::XrlAtomNotFound&) {
	XLOG_ERROR("Atom not found");
	cb->dispatch(XrlError::BAD_ARGS(), 0);
	return;
    }
    cb->dispatch(e, &state);
}

bool
XrlPolicyV0p1Client::send_set_proto_target(
	const char*	dst_xrl_target_name,
	const string&	protocol,
	const string&	target,
	const SetProtoTargetCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "policy/0.1/set_proto_target");
    x.args().add("protocol", protocol);
    x.args().add("target", target);
    return _sender->send(x, callback(this, &XrlPolicyV0p1Client::unmarshall_set_proto_target, cb));
}


/* Unmarshall set_proto_target */
void
XrlPolicyV0p1Client::unmarshall_set_proto_target(
	const XrlError&	e,
	XrlArgs*	a,
	SetProtoTargetCB		cb
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
