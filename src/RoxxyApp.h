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
class RoxxyApp: public CefApp, CefBrowserProcessHandler, CefRenderProcessHandler {
public:
	RoxxyApp();
	virtual ~RoxxyApp();

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
		return this;
	}

	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
		return this;
	}

	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
	                                CefRefPtr<CefFrame> frame,
	                                CefRefPtr<CefV8Context> context) override;

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() override;

private:

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(RoxxyApp)
	
};

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
