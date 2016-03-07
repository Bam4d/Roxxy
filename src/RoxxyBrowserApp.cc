/*
 * CefBrowserHandler.cpp
 *
 *  Created on: 2 Feb 2016
 *      Author: bam4d
 */
#include <iostream>

#include "RoxxyBrowserApp.h"


RoxxyBrowserApp::RoxxyBrowserApp() {
	std::cout << "██████╗  ██████╗ ██╗  ██╗██╗  ██╗██╗   ██╗\n"
				 "██╔══██╗██╔═══██╗╚██╗██╔╝╚██╗██╔╝╚██╗ ██╔╝\n"
				 "██████╔╝██║   ██║ ╚███╔╝  ╚███╔╝  ╚████╔╝ \n"
				 "██╔══██╗██║   ██║ ██╔██╗  ██╔██╗   ╚██╔╝  \n"
				 "██║  ██║╚██████╔╝██╔╝ ██╗██╔╝ ██╗   ██║   \n"
				 "╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   \n\n\n\n";
}

RoxxyBrowserApp::~RoxxyBrowserApp() {
}

void RoxxyBrowserApp::OnContextInitialized() {
	LOG(INFO) << "browser context initialized!!!";
}
