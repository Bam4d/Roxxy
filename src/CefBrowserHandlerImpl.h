/*
 * CefBrowserHandler.h
 *
 *  Created on: 2 Feb 2016
 *      Author: bam4d
 */

#ifndef CEFBROWSERHANDLERIMPL_H_
#define CEFBROWSERHANDLERIMPL_H_

#include "include/cef_client.h"
#include <list>
#include <mutex>

#include "CefBrowserHandler.h"

class BrowserPool;


class CefBrowserHandlerImpl : public CefBrowserHandler {

public:
	CefBrowserHandlerImpl();
	virtual ~CefBrowserHandlerImpl();

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

	void StartBrowserSession(CefRefPtr<CefBrowser> browser, std::string url);

	void Initialize(BrowserPool* browserPool);

	void OnPageLoadExecuted(const CefString& string, int browserId);

private:

	// Controller for the browser pool
	BrowserPool* browserPool_;

	virtual void setBrowserUrl(CefRefPtr<CefBrowser> browser, const CefString& url);

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

	IMPLEMENT_REFCOUNTING(CefBrowserHandlerImpl)

};

#endif /* CEFBROWSERHANDLERIMPL_H_ */
