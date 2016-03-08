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
#include "SourceVisitor.h"

CefBrowserHandler::CefBrowserHandler(BrowserPool* browserPool) {
	// TODO Auto-generated constructor stub
	LOG(INFO) << "starting browser handler";

	browserPool_ = browserPool;

	freeBrowserList_ = new MPMCQueue<int>(browserPool->Size());

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
 * Grab a free browser from the free browser list
 */
CefRefPtr<CefBrowser> CefBrowserHandler::PopFreeBrowser() {
	int browserId;

	freeBrowserList_->blockingRead(browserId);

	return getBrowserById(browserId);
}


/**
 * Get the browser based on it's unique Id
 */
CefRefPtr<CefBrowser> CefBrowserHandler::getBrowserById(int id) {
	for(auto browser : browserList_) {
		if(browser->GetIdentifier() == id) {
			return browser;
		}
	}
	return nullptr;
}

/**
 *
 */
void CefBrowserHandler::StartBrowserSession(int browserId, RenderProxyHandler* renderProxyHandler) {
	LOG(INFO)<< "Starting browser " << browserId << "session";
	setBrowserUrl(getBrowserById(browserId), renderProxyHandler->url);
}

void CefBrowserHandler::setBrowserUrl(CefRefPtr<CefBrowser> browser, const CefString& url) {
	DCHECK(browser != nullptr);

	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI,
				base::Bind(&CefBrowserHandler::setBrowserUrl, this, browser, url));
		return;
	}

	LOG(INFO) << "Setting browser " << browser->GetIdentifier() << " url: " << url.ToString();
	browser->GetMainFrame()->LoadURL(url);
}

/**
 * Handler methods for the browser
 */
void CefBrowserHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	browserList_.push_back(browser);

	setBrowserUrl(browser, "about:blank");

	// Send a ping just to say hi to the renderer process
	CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("ping");
	browser->SendProcessMessage(PID_RENDERER, msg);

	loadCounters_.insert(std::make_pair(browser->GetIdentifier(),0));
	freeBrowserList_->blockingWrite(browser->GetIdentifier());
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

void CefBrowserHandler::EndBrowserSession(int browserId) {
	// Set the browser url once it is loaded to the "about-blank" state
	setBrowserUrl(getBrowserById(browserId), "about:blank");
	freeBrowserList_->blockingWrite(browserId);
}

void CefBrowserHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) {
	if(!isLoading) {

		bool isAboutBlank = browser->GetMainFrame()->GetURL() == "about:blank";

		if(isAboutBlank) {
			return;
		}

		// we have been to the about:blank page and we have loaded our new page
		if(canGoBack && loadCounters_[browser->GetIdentifier()] == 0) {
			LOG(INFO)<< "Executing window.roxxy_loaded(); in browser" << browser->GetIdentifier();
			browser->GetMainFrame()->ExecuteJavaScript("window.roxxy_loaded();", browser->GetMainFrame()->GetURL(),0);
			//browser->GetMainFrame()->GetSource(CefRefPtr<SourceVisitor>(new SourceVisitor(this, browser->GetIdentifier())));
		}
	}
}

void CefBrowserHandler::OnSourceVisited(const CefString& string, int browserId) {
	browserPool_->GetAssignedRenderProxyHandler(browserId)->SendResponse(string);
}

void CefBrowserHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {
}

void CefBrowserHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) {
	loadCounters_[browser->GetIdentifier()]++;
}

void CefBrowserHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {

	loadCounters_[browser->GetIdentifier()]--;
}

bool CefBrowserHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message) {

	if(message->GetName() == "roxxy_loaded") {
		browser->GetMainFrame()->GetSource(CefRefPtr<SourceVisitor>(new SourceVisitor(this, browser->GetIdentifier())));
	} else if(message->GetName() == "pong") {
		LOG(INFO)<<"Recieved pong";
	}

	return true;
}
