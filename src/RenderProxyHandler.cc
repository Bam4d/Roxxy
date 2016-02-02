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

using namespace proxygen;

RenderProxyHandler::RenderProxyHandler(std::shared_ptr<CefBrowserHandler> browserHandler) {
	LOG(INFO) << "ProxyHandler created.";

	browserHandler_ = std::move(browserHandler);

}

RenderProxyHandler::~RenderProxyHandler() {
	LOG(INFO) << "ProxyHandler being destroyed.";
	// TODO Auto-generated destructor stub
}

void RenderProxyHandler::onRequest(std::unique_ptr<HTTPMessage> headers)
		noexcept {

	// Have to store the evb in this object so we can re-use the thread to send responses
	evb = folly::EventBaseManager::get()->getExistingEventBase();

	LOG(INFO)<< "Headers received.";

}

void RenderProxyHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {
	if (requestBody_) {
		requestBody_->prependChain(std::move(body));
	} else {
		requestBody_ = std::move(body);
	}

}

void RenderProxyHandler::onEOM() noexcept {
	LOG(INFO)<< "Body received.";

	// Create browser and then only give the response when we have rendered, probably want to send rendered html

	try {
		dynamic jsonData = BufferUtils::getJson(std::move(requestBody_));
		dynamic url = jsonData["url"];

		LOG(INFO) << "URL is :" << toPrettyJson(url);
		this->url = url.asString().toStdString();

		// Start the browser on "about:blank"
		browserHandler_->StartBrowser(this);

	} catch(...) {
		ResponseBuilder(downstream_).status(400, "BAD_REQUEST")
		.body("bad data sent")
		.sendWithEOM();
	}

}

void RenderProxyHandler::onUpgrade(UpgradeProtocol protocol) noexcept {
	// handler doesn't support upgrades
}

void RenderProxyHandler::requestComplete() noexcept {
	LOG(INFO)<< "Request complete.";
	browserHandler_->CloseBrowser(false);
	delete this;
}

void RenderProxyHandler::onError(ProxygenError err) noexcept {
	LOG(WARNING) << "Request error";
	browserHandler_->CloseBrowser(false);
	delete this;
}

void RenderProxyHandler::SendResponse(std::string response) {
	DCHECK(evb != nullptr);

	evb->runInEventBaseThread([&] () {

		std::stringstream ss;
			ss << "<html><body bgcolor=\"white\">"
					"<h2>DONE</h2></body></html>";

		std::string response = ss.str();

		ResponseBuilder(downstream_).status(200, "OK")
				.body(response)
				//.header("roxxy-url", url_)
				//.header("roxxy-bears","BEARS!")
				//.header("content-length", folly::to<std::string>(response.length()))
				.sendWithEOM();
	});
}


