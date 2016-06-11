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
#include "ResourceFilter.h"


CefBrowserHandlerImpl::CefBrowserHandlerImpl(std::string resourceFilterFolder) {

	if(!resourceFilterFolder.empty()) {
		resourceFilter_ = new ResourceFilter();
		resourceFilter_->LoadFilters(resourceFilterFolder);
	} else {
		resourceFilter_ = nullptr;
	}

	_hostRegex = boost::regex("^(?:https?://)?(?:[^@\\/\\n]+@)?(?:www\\.)?([^:/\\n]+)");
}

CefBrowserHandlerImpl::~CefBrowserHandlerImpl() {
	LOG(INFO) << "killing browser handler";

	if(resourceFilter_ != nullptr) {
		delete resourceFilter_;
	}
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

	ResetBrowser(browser);

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


}

/**
 * End the session with the browser.
 */
void CefBrowserHandlerImpl::ResetBrowser(CefRefPtr<CefBrowser> browser) {
	// Set the browser url once it is loaded to the "about-blank" state
	BrowserSession* browserSession = browserPool_->GetBrowserSessionById(browser->GetIdentifier());
	browserSession->pageUrl = "about:blank";
	browserSession->pageDomain = "";
	browserSession->isLoaded = false;
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
			browserPool_->GetBrowserSessionById(browser->GetIdentifier())->isLoaded = true;

			LOG(INFO)<< "Executing window.roxxy_loaded(); in browser: " << browser->GetIdentifier();
			browser->GetMainFrame()->ExecuteJavaScript("if(!window.cef_loaded) {window.cef_loaded = true; setTimeout(window.roxxy_loaded, 0);}", browser->GetMainFrame()->GetURL(),0);
		}
	}
}

void CefBrowserHandlerImpl::OnPageLoadExecuted(const CefString& htmlContent, int browserId) {
	DCHECK(browserPool_ != nullptr);
	BrowserSession* browserSession = browserPool_->GetBrowserSessionById(browserId);

	browserSession->htmlContent = htmlContent.ToString();

	browserPool_->GetAssignedRenderProxyHandler(browserId)->PageRenderCompleted(browserSession);
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

	bool isLoaded = browserPool_->GetBrowserSessionById(browser->GetIdentifier())->isLoaded;

	if(isLoaded) {
		png_buffer* pngBuffer = &browserPool_->GetBrowserSessionById(browser->GetIdentifier())->pngBuffer;

		// Renders the page image as a png into our browser state png buffer
		RenderPageImage::RenderPNG(buffer, pngBuffer, width, height);
	}
}


/**
 * Handle any javascript dialogs that come from the page
 */
bool CefBrowserHandlerImpl::OnJSDialog(CefRefPtr<CefBrowser> browser,
		                          const CefString& origin_url,
		                          const CefString& accept_lang,
		                          JSDialogType dialog_type,
		                          const CefString& message_text,
		                          const CefString& default_prompt_text,
		                          CefRefPtr<CefJSDialogCallback> callback,
		                          bool& suppress_message) {
	suppress_message = true;
	LOG(INFO)<<"JS DIALOG"<<message_text.ToString();
	return false;
}

/**
 * CefRequestHandler methods
 */
cef_return_value_t CefBrowserHandlerImpl::OnBeforeResourceLoad(
	  CefRefPtr<CefBrowser> browser,
	  CefRefPtr<CefFrame> frame,
	  CefRefPtr<CefRequest> request,
	  CefRefPtr<CefRequestCallback> callback) {
	BrowserSession* browserSession = browserPool_->GetBrowserSessionById(browser->GetIdentifier());

	bool isAboutBlank = browserSession->pageUrl.compare("about:blank") == 0;

	if(resourceFilter_!= nullptr && !isAboutBlank) {
		if(resourceFilter_->ShouldFilterUrl(request->GetURL(), browserSession->pageDomain)) {
			LOG(INFO)<<"Resource "<<request->GetURL().ToString()<<" cancelled";
			return RV_CANCEL;
		}
	}
	// Set the first requested url as the main page url
	if (isAboutBlank) {
		browserSession->pageUrl = request->GetURL().ToString();

		boost::match_results<string::const_iterator> what;
		if( boost::regex_search( browserSession->pageUrl, what, _hostRegex ) ) {
			browserSession->pageDomain = std::string(what[1].first,what[1].second);
		}

		LOG(INFO) << "Setting browser session pageUrl "<<browserSession->pageUrl;
	}

	return RV_CONTINUE;
}

void CefBrowserHandlerImpl::OnResourceRedirect(CefRefPtr<CefBrowser> browser,
							  CefRefPtr<CefFrame> frame,
							  CefRefPtr<CefRequest> request,
							  CefString& new_url) {

	BrowserSession* browserSession = browserPool_->GetBrowserSessionById(browser->GetIdentifier());

	std::string requestUrl = request->GetURL().ToString();

	// If we are redirecting the main url
	if(browserSession->pageUrl.compare(requestUrl) == 0) {
		// Set the final url to the new url
		browserSession->pageUrl = new_url;
	}

	LOG(INFO)<<"Resource "<<request->GetURL().ToString()<<" redirected to "<<new_url.ToString();

}

bool CefBrowserHandlerImpl::OnResourceResponse(CefRefPtr<CefBrowser> browser,
							  CefRefPtr<CefFrame> frame,
							  CefRefPtr<CefRequest> request,
							  CefRefPtr<CefResponse> response) {

	std::string requestUrl = request->GetURL().ToString();

	LOG(INFO)<<"Resource "<<requestUrl<<" status code "<<response->GetStatus();

	BrowserSession* browserSession = browserPool_->GetBrowserSessionById(browser->GetIdentifier());

	// If this is the response for the main url
	if(browserSession->pageUrl.compare(requestUrl) == 0) {
		// Set the statusCode of the session to this response
		browserSession->statusCode = response->GetStatus();
	}

	return false;
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
