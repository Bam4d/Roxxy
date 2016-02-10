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
	browserToHandler_ = new folly::AtomicHashMap<int64_t, int64_t>(browsers);
	handlerToBrowser_ = new folly::AtomicHashMap<int64_t, int64_t>(browsers);

	numBrowsers_ = browsers;

}

BrowserPool::~BrowserPool() {
	// TODO Auto-generated destructor stub
	delete browserHandler_;
	delete browserToHandler_;
	delete handlerToBrowser_;
}

void BrowserPool::Initialize() {

	// Initialize the browser handler
	browserHandler_ = new CefBrowserHandler(this);

	// Start all the browsers for this browser pool
	browserHandler_->Initialize();
}

// Gets a browser instance from the pool
// This will block until a browser is available
CefBrowser* BrowserPool::AssignBrowserSync(RenderProxyHandler* renderProxyHandler) {
	DCHECK(renderProxyHandler != nullptr);
	//TODO: NEED TO LOCK THIS FUNCTION THERE IS A RACE CONDITION
	// TWO ASSIGNS COULD HAPPEN AT THE SAME TIME

	CefBrowser* freeBrowser = browserHandler_->GetFreeBrowser();

	LOG(INFO) << "BROWSER POINTER" << reinterpret_cast<int64_t>(freeBrowser);
	// Then we assign it to the handler
	// Dear god please tell me there is a better way?!...
	browserToHandler_->insert(reinterpret_cast<int64_t>(freeBrowser), reinterpret_cast<int64_t>(renderProxyHandler));
	handlerToBrowser_->insert(reinterpret_cast<int64_t>(renderProxyHandler), reinterpret_cast<int64_t>(freeBrowser));

	return freeBrowser;
}

void BrowserPool::SendResponse(CefBrowser* browser, std::string responseContent) {
	DCHECK(browser != nullptr);
	LOG(INFO) << "BROWSER POINTER" << reinterpret_cast<int64_t>(browser);
	LOG(INFO) << "Sending response " << responseContent;
	RenderProxyHandler* renderProxyHandler = GetAssignedRenderProxyHandler(browser);
	LOG(INFO) << "Sending response " << responseContent;
	renderProxyHandler->SendResponse(responseContent);
}

void BrowserPool::StartBrowserSession(RenderProxyHandler* renderProxyHandler) {
	DCHECK(renderProxyHandler != nullptr);

	// Find the browser we have assigned to this request and then start the session based on the url
	browserHandler_->StartBrowserSession(GetAssignedBrowser(renderProxyHandler), renderProxyHandler);
}

// Get the render proxy handler for a specific browser
RenderProxyHandler* BrowserPool::GetAssignedRenderProxyHandler(CefBrowser* browser){
	DCHECK(browser != nullptr);
	LOG(INFO) << "BROWSER POINTER" << reinterpret_cast<int64_t>(browser);
	auto ret = browserToHandler_->find(reinterpret_cast<int64_t>(browser));
	DCHECK(ret != browserToHandler_->end());
	return reinterpret_cast<RenderProxyHandler*>(ret->second);
}

// Get the assigned browser for the renderProxyHandler
CefBrowser* BrowserPool::GetAssignedBrowser(RenderProxyHandler* renderProxyHandler){
	DCHECK(renderProxyHandler != nullptr);
	auto ret = handlerToBrowser_->find(reinterpret_cast<int64_t>(renderProxyHandler));
	DCHECK(ret != handlerToBrowser_->end());
	return reinterpret_cast<CefBrowser*>(ret->second);
}

int BrowserPool::Size() {
	return numBrowsers_;
}

