/*
 * CefBrowserHandler.h
 *
 *  Created on: 2 Feb 2016
 *      Author: bam4d
 */

#ifndef CEFBROWSERHANDLER_H_
#define CEFBROWSERHANDLER_H_

#include "include/cef_client.h"
#include <list>
#include <mutex>
class BrowserPool;
class RenderProxyHandler;

class CefBrowserHandler : public CefClient,
		public CefDisplayHandler,
		public CefLifeSpanHandler,
		public CefLoadHandler,
		public CefRenderHandler {
public:
	CefBrowserHandler();
	virtual ~CefBrowserHandler();

	// CefRenderHandler methods:
	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() {
		return this;
	}

	virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
		rect = CefRect(0, 0, 1920, 1080);
		return true;
	}

	// CefClient methods:
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override {
		return this;
	}
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
		return this;
	}
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override {
		return this;
	}

	void ResetBrowser(CefRefPtr<CefBrowser> browser);

	void StartBrowserSession(CefRefPtr<CefBrowser> browser, RenderProxyHandler* renderProxyHandler);

	void Initialize(BrowserPool* browserPool);

	void OnPageLoadExecuted(const CefString& string, int browserId);

private:

	// Controller for the browser pool
	BrowserPool* browserPool_;

	void setBrowserUrl(CefRefPtr<CefBrowser> browser, const CefString& url);

	// CefDisplayHandler methods:
	void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;

	// CefLifeSpanHandler methods:
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

	// CefLoadHandler methods:
	virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
			bool isLoading,
			bool canGoBack,
			bool canGoForward) override;

	virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame, ErrorCode errorCode,
			const CefString& errorText, const CefString& failedUrl) override;

	virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame) override;

	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame, int httpStatusCode) override;

	// Paint
	virtual void OnPaint(CefRefPtr<CefBrowser> browser,
            PaintElementType type,
            const RectList& dirtyRects,
            const void* buffer,
            int width, int height) override;

	// Process messages
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
	                                        CefProcessId source_process,
	                                        CefRefPtr<CefProcessMessage> message) override;

	IMPLEMENT_REFCOUNTING(CefBrowserHandler)

};

#endif /* CEFBROWSERHANDLER_H_ */
