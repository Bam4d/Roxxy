/*
 * RenderProxyHandler.cpp
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#include "RenderProxyHandlerImpl.h"

#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <proxygen/lib/utils/CryptUtil.h>
#include <memory>

#include <folly/json.h>
#include <folly/io/async/EventBaseManager.h>
#include <boost/algorithm/string/predicate.hpp>

using folly::dynamic;
using folly::parseJson;
using folly::toJson;

using proxygen::base64Encode;

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

void RenderProxyHandlerImpl::HandleGet() {

	std::string getUrl = request_->getDecodedQueryParam("url");
	std::string path = request_->getPath();

	if(path == "/png") {
		LOG(INFO) << "PNG image requested";
		requestType = PNG;
	} else if(path == "/html") {
		requestType = HTML;
		LOG(INFO) << "HTML page requested";
	} else if(path == "/_status") {
		requestType = STATUS;
		LOG(INFO) << "STATUS page requested";
	} else {
		ResponseBuilder(downstream_)
			.status(404, "NOT_FOUND")
			.body("{\"message\": \"The endpoint you have requested does not exist\"}")
			.sendWithEOM();
		return;
	}

	if(!getUrl.empty() && !boost::starts_with(getUrl, "about:") && !boost::starts_with(getUrl, "chrome://")){
		this->url = getUrl;
		browserPool_->StartBrowserSession(this);
	} else {
		ResponseBuilder(downstream_)
			.status(400, "BAD_REQUEST")
			.body("{\"message\": \"Url get parameter must be set, for example http://localhost:8055?url=http%3A%2F%2Fwww.google.com%0A\"}")
			.sendWithEOM();
	}
}

void RenderProxyHandlerImpl::HandlePost() {
	std::string path = request_->getPath();

	requestType = CUSTOM;
	LOG(INFO) << "CUSTOM endpoint requested";

	dynamic jsonRequest = BufferUtils::GetJson(std::move(requestBody_));

	std::string getUrl = jsonRequest["url"].asString().toStdString();

	if(!getUrl.empty() && !boost::starts_with(getUrl, "about:") && !boost::starts_with(getUrl, "chrome://")){
		this->url = getUrl;
		browserPool_->StartBrowserSession(this);
	} else {
		ResponseBuilder(downstream_)
			.status(400, "BAD_REQUEST")
			.body("{\"message\": \"Url get parameter must be set, for example http://localhost:8055?url=http%3A%2F%2Fwww.google.com%0A\"}")
			.sendWithEOM();
	}
}

void RenderProxyHandlerImpl::onEOM() noexcept {
	DCHECK(browserPool_ != nullptr);

	try {

		switch(*request_->getMethod()){
			case HTTPMethod::GET:
				HandleGet();
				break;
			case HTTPMethod::POST:
				HandlePost();
				break;
			default:
				LOG(INFO) << "Request error";
				ResponseBuilder(downstream_)
					.status(405, "METHOD_NOT_ALLOWED")
					.body("Cannot process request.")
					.sendWithEOM();
				break;
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

void RenderProxyHandlerImpl::PageRenderCompleted(const std::string htmlContent, int statusCode, png_buffer* pngBuffer) {
	switch(requestType) {
			case RequestType::HTML:
			{
				SendHtmlResponse(htmlContent);
				break;
			}
			case RequestType::PNG:
			{
				SendImageResponse(pngBuffer->buffer, pngBuffer->size, "image/png");
				break;
			}
			case RequestType::CUSTOM:
			{
				dynamic response = dynamic::object;

				response["html"] = htmlContent;
				response["statusCode"] = statusCode;
				response["png"] = BufferUtils::Base64Encode(reinterpret_cast<const char*>(pngBuffer->buffer), pngBuffer->size);

				SendCustomResponse(response);
				break;
			}
			case RequestType::STATUS:
				break;
			default:
				break;
		}
}

void RenderProxyHandlerImpl::SendCustomResponse(dynamic jsonResponse) {
	DCHECK(evb != nullptr);

	std::string jsonResponseString = toJson(jsonResponse).toStdString();

	evb->runInEventBaseThread([&, jsonResponseString] () {
			ResponseBuilder(downstream_).status(200, "OK")
					.body(jsonResponseString)
					.header(HTTP_HEADER_CONTENT_TYPE, "application/json")
					.sendWithEOM();
		});
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

void RenderProxyHandlerImpl::SendHtmlResponse(std::string responseContent) {
	DCHECK(evb != nullptr);

	LOG(INFO)<<"Sending html response";

	evb->runInEventBaseThread([&, responseContent] () {

		std::string response = responseContent;

		ResponseBuilder(downstream_).status(200, "OK")
				.body(response)
				.sendWithEOM();
	});
}


