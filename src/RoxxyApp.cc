/*
 * CefBrowserHandler.cpp
 *
 *  Created on: 2 Feb 2016
 *      Author: bam4d
 */

#include "RoxxyApp.h"

#include <string>
#include <iostream>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"

#include "V8CallbackHandler.h"

RoxxyApp::RoxxyApp() {
	std::cout << "██████╗  ██████╗ ██╗  ██╗██╗  ██╗██╗   ██╗\n"
				 "██╔══██╗██╔═══██╗╚██╗██╔╝╚██╗██╔╝╚██╗ ██╔╝\n"
				 "██████╔╝██║   ██║ ╚███╔╝  ╚███╔╝  ╚████╔╝ \n"
				 "██╔══██╗██║   ██║ ██╔██╗  ██╔██╗   ╚██╔╝  \n"
				 "██║  ██║╚██████╔╝██╔╝ ██╗██╔╝ ██╗   ██║   \n"
				 "╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   \n\n\n\n";
}

RoxxyApp::~RoxxyApp() {
}

void RoxxyApp::OnContextInitialized() {
	//LOG(INFO) << "browser context initialized!!!";
}

void RoxxyApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
	                                CefRefPtr<CefFrame> frame,
	                                CefRefPtr<CefV8Context> context) {

	LOG(INFO) << "V8 context created, adding roxxy_loaded function";
	// Generate a JS callback handler for when roxxy is loaded
	CefRefPtr<CefV8Value> object = context->GetGlobal();
	CefRefPtr<CefV8Handler> handler = new V8CallbackHandler();
	CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("roxxy_loaded", handler);
	object->SetValue("roxxy_loaded", func, V8_PROPERTY_ATTRIBUTE_NONE);
}
