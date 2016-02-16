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

	CefRefPtr<CefBrowser> PopFreeBrowser();

	// End the session with the browser.
	void EndBrowserSession(int browserId);

	void StartBrowserSession(int browserId, RenderProxyHandler* renderProxyHandler);

	void Initialize();

	void OnSourceVisited(const CefString& string, int browserId);

private:

	// Controller for the browser pool
	BrowserPool* browserPool_;

	// Manage the browser resources
	typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
	BrowserList browserList_;

	CefRefPtr<CefBrowser> getBrowserById(int id);

	MPMCQueue<int>* freeBrowserList_;

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


	IMPLEMENT_REFCOUNTING(CefBrowserHandler)

};

#endif /* CEFBROWSERHANDLER_H_ */
