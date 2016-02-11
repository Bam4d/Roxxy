/*
 * BrowserPool.cc
 *
 *  Created on: 6 Feb 2016
 *      Author: bam4d
 */

#include "include/cef_browser.h"

/**
 * Local includes
 */
#include "BrowserPool.h"
#include "RenderProxyHandler.h"
#include "CefBrowserHandler.h"



/**
 * The browser pool is a resource manager for CefBrowsers, responsible for allocating browsers from incoming requests
 */
BrowserPool::BrowserPool(int browsers) {

	DCHECK(browsers <= 200);

	int routingMapSize = browsers<10 ? 10:browsers;

	browserIdToHandler_ = new folly::AtomicHashMap<int64_t, int64_t>(routingMapSize);
	handlerToBrowserId_ = new folly::AtomicHashMap<int64_t, int64_t>(routingMapSize);

	numBrowsers_ = browsers;

}

BrowserPool::~BrowserPool() {
	// TODO Auto-generated destructor stub
	delete browserHandler_;
	delete browserIdToHandler_;
	delete handlerToBrowserId_;
}

void BrowserPool::Initialize() {

	// Initialize the browser handler
	browserHandler_ = new CefBrowserHandler(this);

	// Start all the browsers for this browser pool
	browserHandler_->Initialize();
}

// Gets a browser instance from the pool
// This will block until a browser is available
int BrowserPool::AssignBrowserSync(RenderProxyHandler* renderProxyHandler) {
	DCHECK(renderProxyHandler != nullptr);
	//TODO: NEED TO LOCK THIS FUNCTION

	CefRefPtr<CefBrowser> freeBrowser = browserHandler_->PopFreeBrowser();
	int browserId = freeBrowser->GetIdentifier();

	// Then we assign it to the handler
	browserIdToHandler_->insert(browserId, reinterpret_cast<int64_t>(renderProxyHandler));
	handlerToBrowserId_->insert(reinterpret_cast<int64_t>(renderProxyHandler), browserId);
	return browserId;
}

/**
 * The request has finished so release the browser back to the pool
 */
void BrowserPool::ReleaseBrowserSync(RenderProxyHandler* renderProxyHandler) {
	//TODO: NEED TO LOCK THIS FUNCTION

	// Remove from the routing maps
	int freeBrowserId = GetAssignedBrowserId(renderProxyHandler);
	browserIdToHandler_->erase(freeBrowserId);
	handlerToBrowserId_->erase(reinterpret_cast<int64_t>(renderProxyHandler));

	// Add back to the freeBrowser list
	browserHandler_->EndBrowserSession(freeBrowserId);
}

void BrowserPool::SendResponse(int browserId, std::string responseContent) {
	RenderProxyHandler* renderProxyHandler = GetAssignedRenderProxyHandler(browserId);
	renderProxyHandler->SendResponse(responseContent);
}

void BrowserPool::StartBrowserSession(RenderProxyHandler* renderProxyHandler) {
	DCHECK(renderProxyHandler != nullptr);

	// Find the browser we have assigned to this request and then start the session based on the url
	browserHandler_->StartBrowserSession(GetAssignedBrowserId(renderProxyHandler), renderProxyHandler);
}

// Get the render proxy handler for a specific browser
RenderProxyHandler* BrowserPool::GetAssignedRenderProxyHandler(int browserId){
	//DCHECK(browserId != nullptr);
	auto ret = browserIdToHandler_->find(browserId);
	DCHECK(ret != browserIdToHandler_->end());
	return reinterpret_cast<RenderProxyHandler*>(ret->second);
}

// Get the assigned browser for the renderProxyHandler
int BrowserPool::GetAssignedBrowserId(RenderProxyHandler* renderProxyHandler){
	DCHECK(renderProxyHandler != nullptr);
	auto ret = handlerToBrowserId_->find(reinterpret_cast<int64_t>(renderProxyHandler));
	DCHECK(ret != handlerToBrowserId_->end());
	return ret->second;
}

int BrowserPool::Size() {
	return numBrowsers_;
}

