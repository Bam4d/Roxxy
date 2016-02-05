/*
 * RenderProxyHandler.h
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#ifndef RENDERPROXYHANDLER_H_
#define RENDERPROXYHANDLER_H_

#include <folly/Memory.h>
#include <proxygen/httpserver/RequestHandler.h>

#include "CefBrowserHandler.h"

class RenderProxyHandler: public proxygen::RequestHandler {
public:
	RenderProxyHandler(CefRefPtr<CefBrowserHandler> browserHandler);
	virtual ~RenderProxyHandler();

	void SendResponse(std::string response_data);

	// Local storage for the requested URL to load
	std::string url;

private:

	CefRefPtr<CefBrowserHandler> browserHandler_;

	// Want to store a reference to the event base for folly
	folly::EventBase* evb;

	// Local storage for request body buffer
	std::unique_ptr<folly::IOBuf> requestBody_;

	// When there are request headers
	void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers)
			noexcept override;

	// We have a body
	void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

	// We have the end of the message from the client
	void onEOM() noexcept override;

	// NO idea but not important
	void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override;

	// the request is complete
	void requestComplete() noexcept override;

	void onError(proxygen::ProxygenError err) noexcept override;

};

#endif /* RENDERPROXYHANDLER_H_ */
