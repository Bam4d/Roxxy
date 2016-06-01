/*
 * RenderProxyHandler.h
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#ifndef RENDERPROXYHANDLER_H_
#define RENDERPROXYHANDLER_H_

#include <folly/Memory.h>
#include <proxygen/lib/http/HTTPMessage.h>

// Forward declarations
class BrowserPool;

#include "RenderProxyHandler.h"

using folly::dynamic;

class RenderProxyHandlerImpl: public RenderProxyHandler {
public:
	RenderProxyHandlerImpl(BrowserPool* browserHandler);
	virtual ~RenderProxyHandlerImpl();

	void PageRenderCompleted(BrowserSession* browserSession) override;

	const std::string GetRequestedUrl() override {
		return url;
	}

	const RequestType GetRequestType() override {
		return requestType;
	}

	// Local storage for the requested URL to load
	std::string url;

	RequestType requestType = HTML;

	// If this request handler has an assigned browser, if it does not, we cannot process it
	bool hasAssignedBrowser_;

private:

	std::unique_ptr<proxygen::HTTPMessage> request_;

	BrowserPool* browserPool_;

	// Want to store a reference to the event base for folly
	folly::EventBase* evb;

	// Local storage for request body buffer
	std::unique_ptr<folly::IOBuf> requestBody_;

	// When there are request headers
	void onRequest(std::unique_ptr<proxygen::HTTPMessage> request)
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

	void HandleGet();

	void HandlePost();

	void SendErrorResponse(std::string message, int statusCode, std::string statusMessage) override;

	void SendHtmlResponse(std::string response_data) override;

	void SendImageResponse(const void* buffer, size_t contentLength, std::string contentType) override;

	void SendCustomResponse(dynamic jsonResponse) override;

};

#endif /* RENDERPROXYHANDLER_H_ */
