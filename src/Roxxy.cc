/*
 * Roxxy.cc
 *
 *  Created on: 23 Mar 2016
 *      Author: bam4d
 */

#include "Roxxy.h"

#include "include/base/cef_logging.h"
#include <string>

#include "folly/Conv.h"

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include "gflags/gflags.h"

#include "RoxxyBrowserApp.h"
#include "RoxxyRenderApp.h"
#include "RoxxyOtherApp.h"

#include "Server.h"

// Define the input flags
DEFINE_int32(http_port, 8055, "Port to listen on with HTTP protocol");
DEFINE_string(ip, "0.0.0.0", "IP/Hostname to bind to");
DEFINE_int32(threads, 0, "Number of threads to listen on. Numbers <= 0 "
             "will use the number of cores on this machine.");
DEFINE_int32(browsers, 10, "Number of threads to listen on. The default is 10");

Roxxy::Roxxy() {
	// TODO Auto-generated constructor stub

}

Roxxy::~Roxxy() {
	// TODO Auto-generated destructor stub
}

int Roxxy::Run(int argc, char* argv[]) {

	// Provide CEF with command-line arguments.
	CefMainArgs main_args(argc, argv);

	// Parse Chromium command-line flags here
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
	command_line->InitFromArgv(argc, argv);

	CefSettings settings;
	CefRefPtr<CefApp> app;
	if(!command_line->HasSwitch("type")) {

		// Parse roxxy command-line flags here
		gflags::ParseCommandLineFlags(&argc, &argv, false);

		LOG(INFO) << "browser process";
		app = new RoxxyBrowserApp();
	} else {
		const std::string& process_type = command_line->GetSwitchValue("type");
		if (process_type == "renderer" || process_type == "zygote") {
			LOG(INFO) << process_type <<" process";
			app = new RoxxyRenderApp();
		} else {
			LOG(INFO) << "other process";
			app = new RoxxyOtherApp();
		}
	}

	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	int exit_code = CefExecuteProcess(main_args, app, NULL);
	if (exit_code >= 0) {
		LOG(INFO) << "processes exited";
		// The sub-process has completed so return here.
		return exit_code;
	}

	settings.no_sandbox = 1;
	settings.windowless_rendering_enabled = 1;

	LOG(INFO)<< "Initializing cef";
	// Initialize CEF for the browser process.
	CefInitialize(main_args, settings, app, NULL);

	// Only start this on the browser process
	if(!command_line->HasSwitch("type")) {
		Server* server = new Server(FLAGS_threads, FLAGS_http_port, FLAGS_ip, FLAGS_browsers);
		server->StartServer();
	}

	// Run the CEF message loop. This will block until CefQuitMessageLoop() is
	// called.
	LOG(INFO)<< "running cef message loop";
	CefRunMessageLoop();

	LOG(INFO)<< "Shutting down";
	// Shut down CEF.
	CefShutdown();
}

