/*
 * RenderProxyHandlerFactory.cpp
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#include "RenderProxyHandlerFactory.h"
#include "include/base/cef_logging.h"

#include "BrowserPool.h"
#include "CefBrowserHandler.h"
#include "CefBrowserHandlerImpl.h"
#include "RenderProxyHandlerImpl.h"


RenderProxyHandlerFactory::RenderProxyHandlerFactory(int port, int numBrowsers) {
	port_ = port;
	LOG(INFO) << "ProxygenServer server starting....";

	cefBrowserHandler_ = new CefBrowserHandlerImpl();

	// Pool browsers here
	browserPool_ = new BrowserPool(cefBrowserHandler_, numBrowsers);
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
	return new RenderProxyHandlerImpl(browserPool_);
}



