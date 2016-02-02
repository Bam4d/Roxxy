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

#include "include/cef_browser.h"
#include "CefBrowserHandler.h"

using namespace proxygen;

class RenderProxyHandlerFactory : public RequestHandlerFactory {
 public:
	RenderProxyHandlerFactory(int port);
	virtual ~RenderProxyHandlerFactory();

	// RequestHandlerFactory stuff
	virtual void onServerStart(folly::EventBase* evb) noexcept override;
	virtual void onServerStop() noexcept override;
	virtual RequestHandler* onRequest(RequestHandler* handler, HTTPMessage* message) noexcept override;

	private:
		// Port number that the server will listen on
		int port_ = 80;

		std::shared_ptr<CefBrowserHandler> browserHandler_;
};

#endif /* RENDERPROXYHANDLERFACTORY_H_ */
