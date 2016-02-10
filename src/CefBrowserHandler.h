/*
 * CefBrowserHandler.h
 *
 *  Created on: 2 Feb 2016
 *      Author: bam4d
 */

#ifndef CEFBROWSERHANDLER_H_
#define CEFBROWSERHANDLER_H_

#include "include/cef_client.h"
#include "folly/MPMCQueue.h"
#include <list>

class BrowserPool;
class RenderProxyHandler;

using namespace folly;


class CefBrowserHandler : public CefClient,
		public CefDisplayHandler,
		public CefLifeSpanHandler,
		public CefLoadHandler,
		public CefRenderHandler {
public:
	CefBrowserHandler(BrowserPool* browserPool);
	virtual ~CefBrowserHandler();

	// CefRenderHandler methods:
	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() {
		return this;
	}

	virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
		rect = CefRect(0, 0, 0, 0);
		return true;
	}

	virtual void OnPaint(CefRefPtr<CefBrowser>,
			CefRenderHandler::PaintElementType, const RectList&, const void*,
			int, int) {

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

	virtual CefBrowser* GetFreeBrowser();

	// End the session with the browser.
	virtual void EndBrowserSession(CefRefPtr<CefBrowser> browser);

	virtual void StartBrowserSession(CefRefPtr<CefBrowser> browser, RenderProxyHandler* renderProxyHandler);

	virtual void Initialize();

private:

	BrowserPool* browserPool_;
	typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
	BrowserList browserList_;


	MPMCQueue<CefBrowser*>* freeBrowserList_;

	virtual void setBrowserUrl(CefBrowser* browser, const CefString& url);

	// CefDisplayHandler methods:
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;

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


	IMPLEMENT_REFCOUNTING(CefBrowserHandler)

};

#endif /* CEFBROWSERHANDLER_H_ */
