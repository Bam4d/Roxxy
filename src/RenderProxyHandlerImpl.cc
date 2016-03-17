/*
 * RenderProxyHandler.cpp
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#include "RenderProxyHandlerImpl.h"

#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <memory>

#include <folly/json.h>
#include <folly/io/async/EventBaseManager.h>
#include <boost/algorithm/string/predicate.hpp>

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

RenderProxyHandlerImpl::RenderProxyHandlerImpl(BrowserPool* browserPool) {
	//LOG(INFO) << "ProxyHandler created.";

	browserPool_ = browserPool;
}

RenderProxyHandlerImpl::~RenderProxyHandlerImpl() {
	//LOG(INFO) << "ProxyHandler being destroyed.";
	// TODO Auto-generated destructor stub
}

void RenderProxyHandlerImpl::onRequest(std::unique_ptr<HTTPMessage> request)
		noexcept {

	// Have to store the evb in this object so we can re-use the thread to send responses
	evb = folly::EventBaseManager::get()->getExistingEventBase();

	request_ = std::move(request);

	browserPool_->AssignBrowserSync(this);

}

void RenderProxyHandlerImpl::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {
	if (requestBody_) {
		requestBody_->prependChain(std::move(body));
	} else {
		requestBody_ = std::move(body);
	}

}

void RenderProxyHandlerImpl::onEOM() noexcept {
	DCHECK(browserPool_ != nullptr);

	try {

		std::string getUrl = request_->getDecodedQueryParam("url");
		std::string path = request_->getPath();

		if(path == "/png") {
			LOG(INFO) << "PNG image requested";
			requestType = PNG;
		} else if(path == "/html") {
			requestType = HTML;
			LOG(INFO) << "HTML page requested";
		} else {
			ResponseBuilder(downstream_)
				.status(404, "NOT_FOUND")
				.body("Url get parameter must be set, for example http://localhost:8055?url=http%3A%2F%2Fwww.google.com%0A")
				.sendWithEOM();
			return;
		}

		if(!getUrl.empty() && !boost::starts_with(getUrl, "about:") && !boost::starts_with(getUrl, "chrome://")){
			this->url = getUrl;
			browserPool_->StartBrowserSession(this);
		} else {
			ResponseBuilder(downstream_)
				.status(400, "BAD_REQUEST")
				.body("Url get parameter must be set, for example http://localhost:8055?url=http%3A%2F%2Fwww.google.com%0A")
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

void RenderProxyHandlerImpl::onUpgrade(UpgradeProtocol protocol) noexcept {
	// handler doesn't support upgrades
}

void RenderProxyHandlerImpl::requestComplete() noexcept {
	DCHECK(browserPool_ != nullptr);
	LOG(INFO) << "Request complete: " << url << " ... Releasing browser: " << browserPool_->GetAssignedBrowserId(this);
	browserPool_->ReleaseBrowserSync(this);
	delete this;
}

void RenderProxyHandlerImpl::onError(ProxygenError err) noexcept {
	DCHECK(browserPool_ != nullptr);
	LOG(WARNING) << "Request error";
	browserPool_->ReleaseBrowserSync(this);
	delete this;
}

void RenderProxyHandlerImpl::SendImageResponse(const void* buffer, size_t contentLength, std::string contentType) {
	DCHECK(evb != nullptr);

	evb->runInEventBaseThread([&, buffer, contentType, contentLength] () {
		std::unique_ptr<folly::IOBuf> imageBody = std::unique_ptr<folly::IOBuf>(new folly::IOBuf(folly::IOBuf::COPY_BUFFER, (const void*) buffer, contentLength));

		ResponseBuilder(downstream_).status(200, "OK")
				.body(std::move(imageBody))
				.header(HTTP_HEADER_CONTENT_TYPE, contentType)
				.header(HTTP_HEADER_CONTENT_LENGTH, folly::to<std::string>(contentLength))
				.sendWithEOM();
	});
}

void RenderProxyHandlerImpl::SendResponse(std::string responseContent) {
	DCHECK(evb != nullptr);

	LOG(INFO)<<"Sending html response";

	evb->runInEventBaseThread([&, responseContent] () {

		std::string response = responseContent;

		ResponseBuilder(downstream_).status(200, "OK")
				.body(response)
				//.header("roxxy-url", url_)
				//.header("roxxy-bears","BEARS!")
				//.header("content-length", folly::to<std::string>(response.length()))
				.sendWithEOM();
	});
}


