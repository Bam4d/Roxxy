/*
 * RenderProxyHandler.cpp
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#include "RenderProxyHandler.h"

#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <memory>

#include <folly/json.h>
#include <folly/io/async/EventBaseManager.h>

using folly::dynamic;
using folly::parseJson;
using folly::toJson;

/**
 * Local includes
 */
#include "utils/Logging.h"
#include "utils/BufferUtils.h"
#include "BrowserPool.h"

using namespace proxygen;

RenderProxyHandler::RenderProxyHandler(BrowserPool* browserPool) {
	//LOG(INFO) << "ProxyHandler created.";

	browserPool_ = browserPool;
}

RenderProxyHandler::~RenderProxyHandler() {
	//LOG(INFO) << "ProxyHandler being destroyed.";
	// TODO Auto-generated destructor stub
}

void RenderProxyHandler::onRequest(std::unique_ptr<HTTPMessage> request)
		noexcept {

	// Have to store the evb in this object so we can re-use the thread to send responses
	evb = folly::EventBaseManager::get()->getExistingEventBase();

	request_ = std::move(request);

	browserPool_->AssignBrowserSync(this);

}

void RenderProxyHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {
	if (requestBody_) {
		requestBody_->prependChain(std::move(body));
	} else {
		requestBody_ = std::move(body);
	}

}

void RenderProxyHandler::onEOM() noexcept {
	DCHECK(browserPool_ != nullptr);

	try {

		std::string getUrl = request_->getDecodedQueryParam("url");

		if(!getUrl.empty()){
			this->url = getUrl;
			browserPool_->StartBrowserSession(this);
		} else {
			ResponseBuilder(downstream_)
				.status(400, "BAD_REQUEST")
				.body("Url get parameter must be set, for example http://roxxyserver?url=http%3A%2F%2Fwww.google.com%0A")
				.sendWithEOM();
		}

	} catch(...) {
		LOG(INFO) << "Request error";
		ResponseBuilder(downstream_)
			.status(400, "BAD_REQUEST")
			.body("Cannot process request.")
			.sendWithEOM();
	}

}

void RenderProxyHandler::onUpgrade(UpgradeProtocol protocol) noexcept {
	// handler doesn't support upgrades
}

void RenderProxyHandler::requestComplete() noexcept {
	DCHECK(browserPool_ != nullptr);
	LOG(INFO) << "Request complete.";
	browserPool_->ReleaseBrowserSync(this);
	delete this;
}

void RenderProxyHandler::onError(ProxygenError err) noexcept {
	DCHECK(browserPool_ != nullptr);
	LOG(WARNING) << "Request error";
	browserPool_->ReleaseBrowserSync(this);
	delete this;
}

void RenderProxyHandler::SendResponse(std::string responseContent) {
	DCHECK(evb != nullptr);

	int browserId = browserPool_->GetAssignedBrowserId(this);
	LOG(INFO) << "Sending response from " << browserId;

	evb->runInEventBaseThread([&, responseContent] () {

		std::string response = responseContent;

		LOG(INFO) << "Sending response (in thread) from " << browserId;
		ResponseBuilder(downstream_).status(200, "OK")
				.body(response)
				//.header("roxxy-url", url_)
				//.header("roxxy-bears","BEARS!")
				//.header("content-length", folly::to<std::string>(response.length()))
				.sendWithEOM();
	});
}


