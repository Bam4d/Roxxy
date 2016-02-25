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
