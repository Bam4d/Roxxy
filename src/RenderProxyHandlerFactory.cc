/*
 * RenderProxyHandlerFactory.cpp
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#include "RenderProxyHandlerFactory.h"

#include "include/base/cef_logging.h"
#include "RenderProxyHandler.h"


RenderProxyHandlerFactory::RenderProxyHandlerFactory(int port) {
	port_ = port;
	LOG(INFO) << "ProxygenServer server starting....";

	// Do something smart here like pool browsers.
	//Don't want to start and stop browser instances all the time, this is probably inefficient
	browserHandler_ = std::shared_ptr<CefBrowserHandler>(new CefBrowserHandler());
}


RenderProxyHandlerFactory::~RenderProxyHandlerFactory() {

}

void RenderProxyHandlerFactory::onServerStart(folly::EventBase* evb) noexcept {
	LOG(INFO) << "Cef server started, listening for connections on port: " << port_;
}

void RenderProxyHandlerFactory::onServerStop() noexcept {
	LOG(INFO) << "Cef server stopped, no longer listening or connections";
}

RequestHandler* RenderProxyHandlerFactory::onRequest(RequestHandler* handler, HTTPMessage* message) noexcept {
	LOG(INFO) << "request received, need to do something with this request now";

	return new RenderProxyHandler(browserHandler_);
}



