/*
 * CefBrowserHandler.cpp
 *
 *  Created on: 2 Feb 2016
 *      Author: bam4d
 */

#include "include/base/cef_logging.h"
#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include "gflags/gflags.h"

#include "RoxxyBrowserApp.h"
#include "RoxxyRenderApp.h"
#include "RoxxyOtherApp.h"

#include "Server.h"

DEFINE_int32(http_port, 8055, "Port to listen on with HTTP protocol");
//DEFINE_int32(spdy_port, 11001, "Port to listen on with SPDY protocol");
//DEFINE_int32(h2_port, 11002, "Port to listen on with HTTP/2 protocol");
DEFINE_string(ip, "0.0.0.0", "IP/Hostname to bind to");
DEFINE_int32(threads, 0, "Number of threads to listen on. Numbers <= 0 "
             "will use the number of cores on this machine.");

// Entry point function for all processes.
int main(int argc, char* argv[]) {
	// Provide CEF with command-line arguments.
	CefMainArgs main_args(argc, argv);

	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
	command_line->InitFromArgv(argc, argv);

	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	int exit_code = CefExecuteProcess(main_args, NULL, NULL);
	if (exit_code >= 0) {
		// The sub-process has completed so return here.
		return exit_code;
	}

	CefSettings settings;
	CefRefPtr<CefApp> app;
	if(!command_line->HasSwitch("type")) {
		app = new RoxxyBrowserApp();
	} else {
		const std::string& process_type = command_line->GetSwitchValue("type");
		if (process_type == "renderer" || process_type == "zygote") {
			app = new RoxxyRenderApp();
		} else {
			app = new RoxxyOtherApp();
		}
	}

	settings.no_sandbox = 1;
	settings.windowless_rendering_enabled = 1;

	LOG(INFO)<< "Initializing cef";
	// Initialize CEF for the browser process.
	CefInitialize(main_args, settings, app, NULL);

	// Only start this on the browser process
	if(!command_line->HasSwitch("type")) {
		Server* server = new Server(FLAGS_threads, FLAGS_http_port, FLAGS_ip);
		server->StartServer();
	}

	// Run the CEF message loop. This will block until CefQuitMessageLoop() is
	// called.
	LOG(INFO)<< "running cef message loop";
	CefRunMessageLoop();

	LOG(INFO)<< "Shutting down";
	// Shut down CEF.
	CefShutdown();

	return 0;
}
