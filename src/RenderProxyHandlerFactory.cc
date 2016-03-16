/*
 * RenderProxyHandlerFactory.cpp
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#include "RenderProxyHandlerFactory.h"
#include "include/base/cef_logging.h"

#include "RenderProxyHandler.h"
#include "BrowserPool.h"


RenderProxyHandlerFactory::RenderProxyHandlerFactory(int port, int numBrowsers) {
	port_ = port;
	LOG(INFO) << "ProxygenServer server starting....";

	// Pool browsers here
	browserPool_ = new BrowserPool(numBrowsers);
	browserPool_->Initialize();
}


RenderProxyHandlerFactory::~RenderProxyHandlerFactory() {
	LOG(INFO) << "destroying RenderProxyHandlerFactory";
	delete browserPool_;
}

void RenderProxyHandlerFactory::onServerStart(folly::EventBase* evb) noexcept {
	LOG(INFO) << "Cef server started, listening for connections on port: " << port_;

}

void RenderProxyHandlerFactory::onServerStop() noexcept {
	LOG(INFO) << "Cef server stopped, no longer listening or connections";
}

RequestHandler* RenderProxyHandlerFactory::onRequest(RequestHandler* handler, HTTPMessage* message) noexcept {

	// The proxy handler and the browser handler are killed automatically, so we don't need to keep references here
	return new RenderProxyHandler(browserPool_);
}



