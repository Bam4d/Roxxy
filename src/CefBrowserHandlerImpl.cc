/*
 * CefBrowserHandler.cpp
 *
 *  Created on: 2 Feb 2016
 *      Author: bam4d
 */

#include "CefBrowserHandlerImpl.h"

#include <png.h>

/**
 * Includes for CEF
 */
#include "include/cef_browser.h"
#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#include "BrowserPool.h"
#include "RenderProxyHandler.h"
#include "RenderPageImage.h"
#include "SourceVisitor.h"

CefBrowserHandlerImpl::CefBrowserHandlerImpl() {
}

CefBrowserHandlerImpl::~CefBrowserHandlerImpl() {
	LOG(INFO) << "killing browser handler";
}

void CefBrowserHandlerImpl::Initialize(BrowserPool* browserPool) {

	browserPool_ = browserPool;

	LOG(INFO) << "Starting browsers.";

	// Information used when creating the native window.
	CefWindowInfo window_info;
	window_info.SetAsWindowless(kNullWindowHandle, false);

	// Specify CEF browser settings here.
	CefBrowserSettings browser_settings;
	//browser_settings.windowless_frame_rate = 1;
	//browser_settings.application_cache = STATE_DISABLED;
	//browser_settings.image_loading = STATE_DISABLED;

	for(int i = 0; i<browserPool_->Size(); i++) {
		LOG(INFO) << "Starting Browser: " << i;
		CefBrowserHost::CreateBrowser(window_info, this, "about:blank",
				browser_settings, NULL);
	}
}

/**
 *
 */
void CefBrowserHandlerImpl::StartBrowserSession(CefRefPtr<CefBrowser> browser, std::string url) {
	setBrowserUrl(browser, url);
}

void CefBrowserHandlerImpl::setBrowserUrl(CefRefPtr<CefBrowser> browser, const CefString& url) {
	DCHECK(browser != nullptr);

	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI,
				base::Bind(&CefBrowserHandlerImpl::setBrowserUrl, this, browser, url));
		return;
	}

	browser->GetMainFrame()->LoadURL(url);
}

/**
 * Handler methods for the browser
 */
void CefBrowserHandlerImpl::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	LOG(INFO)<<"Created browser, registering";
	browserPool_->RegisterBrowser(browser);

	setBrowserUrl(browser, "about:blank");

	// Send a ping just to say hi to the renderer process
	CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("ping");
	browser->SendProcessMessage(PID_RENDERER, msg);

}

bool CefBrowserHandlerImpl::DoClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();
	LOG(WARNING) << "Closing browser";
	return false;
}

void CefBrowserHandlerImpl::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();
	LOG(WARNING) << "Going to close";
}

void CefBrowserHandlerImpl::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) {
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

/**
 * End the session with the browser.
 */
void CefBrowserHandlerImpl::ResetBrowser(CefRefPtr<CefBrowser> browser) {
	// Set the browser url once it is loaded to the "about-blank" state
	setBrowserUrl(browser, "about:blank");

}

void CefBrowserHandlerImpl::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) {
	//LOG(INFO)<< "Browser: " << browser->GetIdentifier() << "Loading state changed:" << isLoading << canGoBack << canGoForward;
	if(!isLoading) {

		bool isAboutBlank = browser->GetMainFrame()->GetURL() == "about:blank";

		if(isAboutBlank) {
			return;
		}

		// we have been to the about:blank page and we have loaded our new page
		if(canGoBack) {
			browserPool_->GetBrowserStateById(browser->GetIdentifier())->isLoaded = true;

			LOG(INFO)<< "Executing window.roxxy_loaded(); in browser: " << browser->GetIdentifier();
			browser->GetMainFrame()->ExecuteJavaScript("if(!window.cef_loaded) {window.cef_loaded = true; setTimeout(window.roxxy_loaded, 500);}", browser->GetMainFrame()->GetURL(),0);
		}
	}
}

void CefBrowserHandlerImpl::OnPageLoadExecuted(const CefString& string, int browserId) {
	DCHECK(browserPool_ != nullptr);

	switch(browserPool_->GetAssignedRenderProxyHandler(browserId)->GetRequestType()) {
		case HTML:
			browserPool_->GetAssignedRenderProxyHandler(browserId)->SendResponse(string);
			break;
		case PNG:
			png_buffer* pngBuffer = &browserPool_->GetBrowserStateById(browserId)->pngBuffer;

			LOG(INFO)<<"pngBuffer"<<pngBuffer;

			browserPool_->GetAssignedRenderProxyHandler(browserId)->SendImageResponse(pngBuffer->buffer, pngBuffer->size, "image/png");
			break;
	}
}

void CefBrowserHandlerImpl::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) {
}

void CefBrowserHandlerImpl::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
}


/**
 * Handle paint, events, such as generating a PNG
 */
void CefBrowserHandlerImpl::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
		const RectList& dirtyRects, const void* buffer, int width, int height) {

	bool isLoaded = browserPool_->GetBrowserStateById(browser->GetIdentifier())->isLoaded;

	if(isLoaded) {
		png_buffer* pngBuffer = &browserPool_->GetBrowserStateById(browser->GetIdentifier())->pngBuffer;

		// Renders the page image as a png into our browser state png buffer
		RenderPageImage::RenderPNG(buffer, pngBuffer, width, height);
	}
}




/**
 * Handle process messages
 */
bool CefBrowserHandlerImpl::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message) {

	if(message->GetName() == "roxxy_loaded") {
		browser->GetMainFrame()->GetSource(CefRefPtr<SourceVisitor>(new SourceVisitor((CefBrowserHandler*)this, browser->GetIdentifier())));
	} else if(message->GetName() == "pong") {
		LOG(INFO)<< "Browser: " << browser->GetIdentifier() << " ready!";
	}

	return true;
}
