/*
 * CefBrowserHandler.cpp
 *
 *  Created on: 2 Feb 2016
 *      Author: bam4d
 */

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_

#include "include/cef_app.h"

// Implement application-level callbacks for the browser process.
class RoxxyBrowserApp: public CefApp, CefBrowserProcessHandler {
public:
	RoxxyBrowserApp();
	virtual ~RoxxyBrowserApp();

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
		return this;
	}

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() override;


	virtual void OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info) override {
		LOG(INFO) << "render process created";
	}


private:

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(RoxxyBrowserApp)
	
};

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
