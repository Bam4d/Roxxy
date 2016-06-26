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

	hasAssignedBrowser_ = browserPool_->AssignBrowserSync(this) > 0;

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
		SendErrorResponse("The endpoint you have requested does not exist", 404, "NOT_FOUND");
		return;
	}

	if(!getUrl.empty() && !boost::starts_with(getUrl, "about:") && !boost::starts_with(getUrl, "chrome://")){
		this->url = getUrl;
		browserPool_->StartBrowserSession(this);
	} else {
		SendErrorResponse("Url parameter must be set, for example http://localhost:8055?url=http%3A%2F%2Fwww.google.com%0A", 400, "BAD_REQUEST");
	}
}

void RenderProxyHandlerImpl::HandlePost() {
	std::string path = request_->getPath();

	requestType = CUSTOM;
	LOG(INFO) << "CUSTOM endpoint requested";

	dynamic jsonRequest = BufferUtils::GetJson(std::move(requestBody_));

	std::string getUrl = jsonRequest["url"].asString();

	if(!getUrl.empty() && !boost::starts_with(getUrl, "about:") && !boost::starts_with(getUrl, "chrome://")){
		this->url = getUrl;

		int browserId = browserPool_->GetAssignedBrowserId(this);
		BrowserSession* session = browserPool_->GetBrowserSessionById(browserId);

		// Set the browser session variables
		if(jsonRequest["png"] != nullptr) {
			session->pngRequested = jsonRequest["png"].asBool();
		}

		browserPool_->StartBrowserSession(this);
	} else {
		SendErrorResponse("Url get parameter must be set, for example http://localhost:8055?url=http%3A%2F%2Fwww.google.com%0A", 400, "BAD_REQUEST");
	}
}

void RenderProxyHandlerImpl::onEOM() noexcept {
	DCHECK(browserPool_ != nullptr);

	// If we did not assign a browser to this request, then we do not handle it currently
	if(!hasAssignedBrowser_) {
		SendErrorResponse("No browsers free currently, please wait and try again.", 420, "NO_FREE_BROWSERS");
		return;
	}

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
				SendErrorResponse("Only GET and POST are supported", 405, "METHOD_NOT_ALLOWED");
				break;
		}

	} catch(...) {
		LOG(INFO) << "Request error";
		SendErrorResponse("Cannot process request", 400, "BAD_REQUEST");
	}

}

void RenderProxyHandlerImpl::onUpgrade(UpgradeProtocol protocol) noexcept {
	// handler doesn't support upgrades
}

void RenderProxyHandlerImpl::requestComplete() noexcept {
	DCHECK(browserPool_ != nullptr);
	if(hasAssignedBrowser_) {
		LOG(INFO) << "Request complete: " << url << " ... Releasing browser: " << browserPool_->GetAssignedBrowserId(this);
		browserPool_->ReleaseBrowserSync(this);
	}
	delete this;
}

void RenderProxyHandlerImpl::onError(ProxygenError err) noexcept {
	DCHECK(browserPool_ != nullptr);
	if(hasAssignedBrowser_) {
		LOG(WARNING) << "Request error";
		browserPool_->ReleaseBrowserSync(this);
	}
	delete this;
}

void RenderProxyHandlerImpl::PageRenderCompleted(BrowserSession* browserSession) {
	switch(requestType) {
			case RequestType::HTML:
			{
				SendHtmlResponse(browserSession->htmlContent);
				break;
			}
			case RequestType::PNG:
			{
				SendImageResponse(browserSession->pngBuffer.buffer, browserSession->pngBuffer.size, "image/png");
				break;
			}
			case RequestType::CUSTOM:
			{
				LOG(INFO)<<"Building custom response";
				dynamic response = dynamic::object();

				//LOG(INFO)<<"Building custom response"<<browserSession->htmlContent;
				response["html"] = browserSession->htmlContent;
				response["statusCode"] = browserSession->statusCode;

				if(browserSession->pngRequested) {
					response["png"] = proxygen::base64Encode(folly::ByteRange(reinterpret_cast<const unsigned char*>(browserSession->pngBuffer.buffer), browserSession->pngBuffer.size));
				}

				SendCustomResponse(response);
				break;
			}
			case RequestType::STATUS:
				break;
			default:
				break;
		}
}

void RenderProxyHandlerImpl::SendErrorResponse(std::string message, int statusCode, std::string statusMessage) {
	ResponseBuilder(downstream_)
			.status(statusCode, statusMessage)
			.body("{\"message\": \"" + message + "\"}")
			.closeConnection()
			.sendWithEOM();
}

void RenderProxyHandlerImpl::SendCustomResponse(dynamic jsonResponse) {
	DCHECK(evb != nullptr);

	std::string jsonResponseString = toJson(jsonResponse);

	LOG(INFO)<<"Sending response";
	bool result = evb->runInEventBaseThreadAndWait([&, jsonResponseString] () {

			ResponseBuilder(downstream_).status(200, "OK")
					.body(jsonResponseString)
					.header(HTTP_HEADER_CONTENT_TYPE, "application/json")
					.sendWithEOM();
		});

	DCHECK(result);
}

void RenderProxyHandlerImpl::SendImageResponse(const void* buffer, size_t contentLength, std::string contentType) {
	DCHECK(evb != nullptr);

	LOG(INFO)<<"Sending response";
	bool result = evb->runInEventBaseThread([&, buffer, contentType, contentLength] () {
		std::unique_ptr<folly::IOBuf> imageBody = std::unique_ptr<folly::IOBuf>(new folly::IOBuf(folly::IOBuf::COPY_BUFFER, (const void*) buffer, contentLength));

		ResponseBuilder(downstream_).status(200, "OK")
				.body(std::move(imageBody))
				.header(HTTP_HEADER_CONTENT_TYPE, contentType)
				.header(HTTP_HEADER_CONTENT_LENGTH, folly::to<std::string>(contentLength))
				.sendWithEOM();
	});

	DCHECK(result);
}

void RenderProxyHandlerImpl::SendHtmlResponse(std::string responseContent) {
	DCHECK(evb != nullptr);

	LOG(INFO)<<"Sending response";
	bool result = evb->runInEventBaseThread([&, responseContent] () {
		ResponseBuilder(downstream_).status(200, "OK")
				.body(responseContent)
				.sendWithEOM();
	});

	DCHECK(result);
}


