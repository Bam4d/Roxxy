/*
 * RenderProxyHandlerFactory.h
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#ifndef RENDERPROXYHANDLERFACTORY_H_
#define RENDERPROXYHANDLERFACTORY_H_

#include <folly/Memory.h>
#include <folly/Portability.h>
#include <folly/io/async/EventBaseManager.h>
#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include "include/wrapper/cef_helpers.h"

class BrowserPool;
class CefBrowserHandler;

using namespace proxygen;

class RenderProxyHandlerFactory : public RequestHandlerFactory {
 public:
	RenderProxyHandlerFactory(int port, int numBrowsers, std::string resourceFilterFilename);
	virtual ~RenderProxyHandlerFactory();

	// RequestHandlerFactory stuff
	virtual void onServerStart(folly::EventBase* evb) noexcept override;
	virtual void onServerStop() noexcept override;
	virtual RequestHandler* onRequest(RequestHandler* handler, HTTPMessage* message) noexcept override;

	private:
		// Port number that the server will listen on
		CefRefPtr<CefBrowserHandler> cefBrowserHandler_;
		int port_ = 80;
		BrowserPool* browserPool_;
};

#endif /* RENDERPROXYHANDLERFACTORY_H_ */
