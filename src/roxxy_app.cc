// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "roxxy_app.h"

#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"

RoxxyApp::RoxxyApp() {
}

RoxxyApp::~RoxxyApp() {
}

void RoxxyApp::OnContextInitialized() {
	CEF_REQUIRE_UI_THREAD();

}
