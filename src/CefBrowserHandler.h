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

#include "RenderPageImage.h"

class BrowserPool;
class RenderProxyHandler;

using namespace folly;


/**
 * Structure containing state information for a browser.
 */
struct BrowserState {
	bool isLoaded = false;
	png_buffer pngBuffer;
};


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

	CefRefPtr<CefBrowser> PopFreeBrowser();

	// End the session with the browser.
	void EndBrowserSession(int browserId);

	void StartBrowserSession(int browserId, RenderProxyHandler* renderProxyHandler);

	void Initialize();

	void OnPageLoadExecuted(const CefString& string, int browserId);

private:

	// Controller for the browser pool
	BrowserPool* browserPool_;

	//Load start and end counters for each browser
	std::map<int, BrowserState> browserState_;

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
