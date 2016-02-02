// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_

#include "include/cef_app.h"

// Implement application-level callbacks for the browser process.
class RoxxyApp: public CefApp, public CefBrowserProcessHandler {
public:
	RoxxyApp();
	virtual ~RoxxyApp();

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
		return this;
	}

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() override;

private:

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(RoxxyApp)
	
};

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
