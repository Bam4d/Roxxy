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

CefBrowserHandler::CefBrowserHandler() {
	// TODO Auto-generated constructor stub

}

CefBrowserHandler::~CefBrowserHandler() {
	// TODO Auto-generated destructor stub
}

void CefBrowserHandler::StartBrowser(RenderProxyHandler* renderProxyHandler) {

	// the render proxy will destroy itself when data is sent,
	// so we don't need to do anything "smart" with pointers here
	renderProxyHandler_ = renderProxyHandler;

	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI,
				base::Bind(&CefBrowserHandler::StartBrowser, this, renderProxyHandler));
		return;
	}

	LOG(INFO) << "Starting browser.";

	// Information used when creating the native window.
	CefWindowInfo window_info;
	window_info.SetAsWindowless(kNullWindowHandle, false);

	// Specify CEF browser settings here.
	CefBrowserSettings browser_settings;
	browser_settings.windowless_frame_rate = 1;
	browser_settings.application_cache = STATE_DISABLED;
	browser_settings.image_loading = STATE_DISABLED;

	// Create our browser here
	CefBrowserHost::CreateBrowser(window_info, this, "about:blank",
			browser_settings, NULL);
}

void CefBrowserHandler::setBrowserUrl(const CefString& url) {
	DCHECK(browser_ != NULL);

	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI,
				base::Bind(&CefBrowserHandler::setBrowserUrl, this, url));
		return;
	}

	// have to validate the url here!!
	browser_->GetMainFrame()->LoadURL(url);
}

/**
 * Handler methods for the browser
 */
void CefBrowserHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	DCHECK(browser_ == NULL);
	browser_ = browser;
}

bool CefBrowserHandler::DoClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	LOG(WARNING) << "Closing browser";

	// Set a flag to indicate that the window close should be allowed.
	is_closing_ = true;

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

	frame->LoadString(ss.str(), failedUrl);
}

void CefBrowserHandler::CloseBrowser(bool force_close) {
	if(browser_ == nullptr) return;

	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI,
				base::Bind(&CefBrowserHandler::CloseBrowser, this,
						force_close));
		return;
	}

	LOG(INFO)<< "Closing browser.";
	browser_->GetHost()->CloseBrowser(force_close);
}

void CefBrowserHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) {
	LOG(INFO) << "Loading state change";
	LOG(INFO) << "isLoading " << isLoading;
	LOG(INFO) << "canGoBack " << canGoBack;
	LOG(INFO) << "canGoForward " << canGoForward;

	if(!isLoading) {

		// We load to about blank to zero the state of the browser
		// TODO: is this necessary?
		LOG(INFO) << "current URL: " << browser->GetMainFrame()->GetURL().ToString();
		bool isAboutBlank = browser->GetMainFrame()->GetURL() == "about:blank";

		if(isAboutBlank) {
				// Set the browser url once it is loaded to the "about-blank" state
				setBrowserUrl(renderProxyHandler_->url);
			return;
		}

		// we have been to the about:blank page and we have loaded our new page
		if(canGoBack) {
			renderProxyHandler_->SendResponse("stuff here");
		}
	}
}

void CefBrowserHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {
	LOG(INFO)<< "Title has changed";
}

void CefBrowserHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) {
	//frame->GetSource(new )
	LOG(INFO)<< "OnLoadStart test";
}

void CefBrowserHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
	LOG(INFO)<< "OnLoadEnd test";
}
