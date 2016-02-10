/*
 * CefBrowserHandler.cpp
 *
 *  Created on: 2 Feb 2016
 *      Author: bam4d
 */

#include "CefBrowserHandler.h"

/**
 * Includes for CEF
 */
#include "include/cef_browser.h"
#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#include "RenderProxyHandler.h"
#include "BrowserPool.h"

CefBrowserHandler::CefBrowserHandler(BrowserPool* browserPool) {
	// TODO Auto-generated constructor stub
	LOG(INFO) << "starting browser handler";

	browserPool_ = browserPool;

	freeBrowserList_ = new MPMCQueue<CefBrowser*>(browserPool->Size());

}

CefBrowserHandler::~CefBrowserHandler() {
	// TODO Auto-generated destructor stub
	LOG(INFO) << "killing browser handler";

	delete freeBrowserList_;
}

void CefBrowserHandler::Initialize() {
	LOG(INFO) << "Starting browsers.";

	// Information used when creating the native window.
	CefWindowInfo window_info;
	window_info.SetAsWindowless(kNullWindowHandle, false);

	// Specify CEF browser settings here.
	CefBrowserSettings browser_settings;
	browser_settings.windowless_frame_rate = 1;
	browser_settings.application_cache = STATE_DISABLED;
	browser_settings.image_loading = STATE_DISABLED;

	for(int i = 0; i<browserPool_->Size(); i++) {
		LOG(INFO) << "Starting Browser: " << i;
		CefBrowserHost::CreateBrowser(window_info, this, "about:blank",
				browser_settings, NULL);
	}
}

/**
 * Return a plain old pointer to the browser we have grabbed from the queue
 */
CefBrowser* CefBrowserHandler::GetFreeBrowser() {
	CefBrowser* browser;
	freeBrowserList_->blockingRead(browser);
	return browser;
}

/**
 *
 */
void CefBrowserHandler::StartBrowserSession(CefRefPtr<CefBrowser> browser, RenderProxyHandler* renderProxyHandler) {

	setBrowserUrl(browser, renderProxyHandler->url);
}

void CefBrowserHandler::setBrowserUrl(CefBrowser* browser, const CefString& url) {
	DCHECK(browser != NULL);

	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI,
				base::Bind(&CefBrowserHandler::setBrowserUrl, this, browser, url));
		return;
	}

	LOG(INFO) << "Setting browser url: " << url.ToString();
	browser->GetMainFrame()->LoadURL(url);

}

/**
 * Handler methods for the browser
 */
void CefBrowserHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	browserList_.push_back(browser);
	freeBrowserList_->blockingWrite(browser.get());
	setBrowserUrl(browser, "about:blank");
}

bool CefBrowserHandler::DoClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	LOG(WARNING) << "Closing browser";

	return false;
}

void CefBrowserHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	LOG(WARNING) << "Going to close";
}

void CefBrowserHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) {
	CEF_REQUIRE_UI_THREAD();
	//DCHECK(renderProxyHandler_ != nullptr);
	LOG(WARNING) << "LOAD ERROR";

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Display a load error message.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
			"<h2>CefBrowserHandler Failed to load URL " << std::string(failedUrl)
			<< " with error " << std::string(errorText) << " (" << errorCode
			<< ").</h2></body></html>";


	//renderProxyHandler_->SendResponse(ss.str());
}

void CefBrowserHandler::EndBrowserSession(CefRefPtr<CefBrowser> browser) {
	setBrowserUrl(browser, "about:blank");
}

void CefBrowserHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) {
	//LOG(INFO) << "Loading state change";
	//LOG(INFO) << "isLoading " << isLoading;
	//LOG(INFO) << "canGoBack " << canGoBack;
	//LOG(INFO) << "canGoForward " << canGoForward;
	//DCHECK(renderProxyHandler_ != nullptr);
	if(!isLoading) {

		// We load to about blank to zero the state of the browser
		// TODO: is this necessary?
		LOG(INFO) << "current URL: " << browser->GetMainFrame()->GetURL().ToString();
		bool isAboutBlank = browser->GetMainFrame()->GetURL() == "about:blank";

		if(isAboutBlank) {
				// Set the browser url once it is loaded to the "about-blank" state
				//setBrowserUrl(renderProxyHandler_->url);
			return;
		}

		// we have been to the about:blank page and we have loaded our new page
		if(canGoBack) {
			browserPool_->SendResponse(browser.get(), "stuff here");
		}
	}
}

void CefBrowserHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {
	//LOG(INFO)<< "Title has changed";
}

void CefBrowserHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) {
	//frame->GetSource(new )
	//LOG(INFO)<< "OnLoadStart test";
}

void CefBrowserHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
	//LOG(INFO)<< "OnLoadEnd test";
}
