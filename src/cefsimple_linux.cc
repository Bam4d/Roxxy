// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/base/cef_logging.h"
#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"

#include <folly/Memory.h>
#include <folly/Portability.h>
#include <folly/io/async/EventBaseManager.h>
#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>

#include "roxxy_app.h"
#include "RenderProxyHandlerFactory.h"

using folly::EventBase;
using folly::EventBaseManager;
using folly::SocketAddress;

using namespace proxygen;
using Protocol = HTTPServer::Protocol;

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

	// start google logging here

	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	int exit_code = CefExecuteProcess(main_args, NULL, NULL);
	if (exit_code >= 0) {
		// The sub-process has completed so return here.
		return exit_code;
	}

	// Specify CEF global settings here.
	CefSettings settings;

	// SimpleApp implements application-level callbacks for the browser process.
	// It will create the first browser instance in OnContextInitialized() after
	// CEF has initialized.
	CefRefPtr<RoxxyApp> app(new RoxxyApp);

	// Start the server stuff here

	std::vector<HTTPServer::IPConfig> IPs = {
		{	SocketAddress(FLAGS_ip, FLAGS_http_port, true), Protocol::HTTP},
		//{	SocketAddress(FLAGS_ip, FLAGS_spdy_port, true), Protocol::SPDY},
		//{	SocketAddress(FLAGS_ip, FLAGS_h2_port, true), Protocol::HTTP2},
	};

	if (FLAGS_threads <= 0) {
		FLAGS_threads = sysconf(_SC_NPROCESSORS_ONLN);
		CHECK(FLAGS_threads > 0);
	}

	HTTPServerOptions options;
	options.threads = static_cast<size_t>(FLAGS_threads);
	options.idleTimeout = std::chrono::milliseconds(30000);
	options.shutdownOn = {SIGINT, SIGTERM};
	options.enableContentCompression = true;
	options.handlerFactories = RequestHandlerChain()
			.addThen<RenderProxyHandlerFactory>(FLAGS_http_port)
			.build();

	HTTPServer server(std::move(options));
	server.bind(IPs);

	// Start HTTPServer mainloop in a separate thread
	std::thread t([&] () {
		server.start();
	});

	LOG(INFO)<< "initializing cef";
	// Initialize CEF for the browser process.
	CefInitialize(main_args, settings, app.get(), NULL);

	// Run the CEF message loop. This will block until CefQuitMessageLoop() is
	// called.
	LOG(INFO)<< "running cef message loop";
	CefRunMessageLoop();

	LOG(INFO)<< "Shutting down";
	// Shut down CEF.
	CefShutdown();

	return 0;
}
