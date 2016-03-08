/*
 * V8CallbackHandler.cc
 *
 *  Created on: 26 Feb 2016
 *      Author: bam4d
 */

#include "include/cef_process_message.h"


#include "V8CallbackHandler.h"

V8CallbackHandler::V8CallbackHandler() {
	// TODO Auto-generated constructor stub

}

V8CallbackHandler::~V8CallbackHandler() {
	// TODO Auto-generated destructor stub
}

bool V8CallbackHandler::Execute(const CefString& name,
                       CefRefPtr<CefV8Value> object,
                       const CefV8ValueList& arguments,
                       CefRefPtr<CefV8Value>& retval,
                       CefString& exception) {

	//LOG(INFO) << "callback handler fired";
	if(name == "roxxy_loaded") {
		CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();

		LOG(INFO) << "roxxy page load script has run in browser " << context->GetBrowser()->GetIdentifier();

		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("roxxy_loaded");

		context->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
		return true;
	}

	return false;
}
