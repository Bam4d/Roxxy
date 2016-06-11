/*
 * CefBrowserHandler.h
 *
 *  Created on: 2 Feb 2016
 *      Author: bam4d
 */

#ifndef CEFBROWSERHANDLER_H_
#define CEFBROWSERHANDLER_H_

#include "include/cef_client.h"

class BrowserPool;
class RenderProxyHandlerImpl;

/**
 * Wrapper for the browser handler interface, can then mock this interface for testing
 */
class CefBrowserHandler : public CefClient,
		public CefDisplayHandler,
		public CefLifeSpanHandler,
		public CefLoadHandler,
		public CefRenderHandler,
		public CefRequestHandler,
		public CefJSDialogHandler {
public:
	virtual ~CefBrowserHandler() {};

	// CefRenderHandler methods:
	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() = 0;

	virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) = 0;

	// CefClient methods:
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override = 0;

	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override = 0;

	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override = 0;

	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override = 0;

	virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override = 0;

	virtual void ResetBrowser(CefRefPtr<CefBrowser> browser) = 0;

	virtual void StartBrowserSession(CefRefPtr<CefBrowser> browser, std::string url) = 0;

	virtual void Initialize(BrowserPool* browserPool) = 0;

	virtual void OnPageLoadExecuted(const CefString& string, int browserId) = 0;

protected:

	virtual void setBrowserUrl(CefRefPtr<CefBrowser> browser, const CefString& url) = 0;

	/**
	 * CefLifeSpanHandler methods:
	 */
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override = 0;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) override = 0;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override = 0;

	/**
	 * CefLoadHandler methods:
	 */
	virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
			bool isLoading,
			bool canGoBack,
			bool canGoForward) override = 0;

	virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame, ErrorCode errorCode,
			const CefString& errorText, const CefString& failedUrl) override  = 0;

	virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame) override = 0;

	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame, int httpStatusCode) override = 0;

	/**
	 * CefJSDialogHandler methods
	 */
	virtual bool OnJSDialog(CefRefPtr<CefBrowser> browser,
	                          const CefString& origin_url,
	                          const CefString& accept_lang,
	                          JSDialogType dialog_type,
	                          const CefString& message_text,
	                          const CefString& default_prompt_text,
	                          CefRefPtr<CefJSDialogCallback> callback,
	                          bool& suppress_message) override = 0;


	/**
	 * CefRequestHandler methods
	 */
	virtual ReturnValue OnBeforeResourceLoad(
		  CefRefPtr<CefBrowser> browser,
		  CefRefPtr<CefFrame> frame,
		  CefRefPtr<CefRequest> request,
		  CefRefPtr<CefRequestCallback> callback) override = 0;

	virtual void OnResourceRedirect(CefRefPtr<CefBrowser> browser,
								  CefRefPtr<CefFrame> frame,
								  CefRefPtr<CefRequest> request,
								  CefString& new_url) override = 0;

	virtual bool OnResourceResponse(CefRefPtr<CefBrowser> browser,
								  CefRefPtr<CefFrame> frame,
								  CefRefPtr<CefRequest> request,
								  CefRefPtr<CefResponse> response) override = 0;


	// Paint
	virtual void OnPaint(CefRefPtr<CefBrowser> browser,
            PaintElementType type,
            const RectList& dirtyRects,
            const void* buffer,
            int width, int height) override = 0;

	// Process messages
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
	                                        CefProcessId source_process,
	                                        CefRefPtr<CefProcessMessage> message) override = 0;

};

#endif /* CEFBROWSERHANDLER_H_ */
