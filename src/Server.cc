/*
 * Server.cc
 *
 *  Created on: 7 Mar 2016
 *      Author: bam4d
 */

#include <folly/Memory.h>
#include <folly/Portability.h>
#include <folly/io/async/EventBaseManager.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>

#include "RenderProxyHandlerFactory.h"

#include "Server.h"

using folly::EventBase;
using folly::EventBaseManager;
using folly::SocketAddress;


using Protocol = HTTPServer::Protocol;


Server::Server(int threads, int port, std::string ip, int numBrowsers, std::string resourceFilterFilename) {
	this->port = port;
	this->ip = ip;
	this->threads = threads;

	IPs = std::vector<HTTPServer::IPConfig>({
			{	SocketAddress(ip, port, true), Protocol::HTTP},
	});

	if (threads <= 0) {
		threads = sysconf(_SC_NPROCESSORS_ONLN);
		CHECK(threads > 0);
	}

	HTTPServerOptions options;
	options.threads = static_cast<size_t>(threads);
	options.idleTimeout = std::chrono::milliseconds(30000);
	options.shutdownOn = {SIGINT, SIGTERM};
	options.enableContentCompression = true;
	options.contentCompressionLevel = 4;
	options.handlerFactories = RequestHandlerChain()
			.addThen<RenderProxyHandlerFactory>(port, numBrowsers, resourceFilterFilename)
			.build();

	server = new HTTPServer(std::move(options));
	server->bind(IPs);
}

Server::~Server() {
	// TODO Auto-generated destructor stub
	serverListenerThread.join();
	delete server;

}

void Server::StartServer() {

	// Start HTTPServer mainloop in a separate thread
	serverListenerThread = std::thread([&] () {
		server->start();
	});
}

