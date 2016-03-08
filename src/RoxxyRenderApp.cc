/*
 * RoxxyRenderApp.cc
 *
 *  Created on: 7 Mar 2016
 *      Author: bam4d
 */


#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"

#include "V8CallbackHandler.h"
#include "RoxxyRenderApp.h"



RoxxyRenderApp::RoxxyRenderApp() {
	// TODO Auto-generated constructor stub

}

RoxxyRenderApp::~RoxxyRenderApp() {
	// TODO Auto-generated destructor stub
}

void RoxxyRenderApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
	                                CefRefPtr<CefFrame> frame,
	                                CefRefPtr<CefV8Context> context) {

	//LOG(INFO) << "V8 context created, adding roxxy_loaded function";
	// Generate a JS callback handler for when roxxy is loaded
	CefRefPtr<CefV8Value> object = context->GetGlobal();
	CefRefPtr<CefV8Handler> handler = new V8CallbackHandler();
	CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("roxxy_loaded", handler);
	object->SetValue("roxxy_loaded", func, V8_PROPERTY_ATTRIBUTE_NONE);
}

bool RoxxyRenderApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) {

	//LOG(INFO) << "Render process message recieved";

	if(message->GetName() == "ping"){
		LOG(INFO)<< "render: ping!";
		CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("pong");
		browser->SendProcessMessage(PID_BROWSER, msg);
	}

	return true;
}

