/*
 * Mocks.cc
 *
 *  Created on: 23 Mar 2016
 *      Author: bam4d
 */

#ifndef ROXXY_BUILD

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../BrowserPool.h"
#include "../../CefBrowserHandler.h"
#include "../../RenderProxyHandler.h"

using namespace testing;

class MockRenderProxyHandler : public RenderProxyHandler {
public:
	MOCK_METHOD1(PageRenderCompleted,
				void(BrowserSession* browserSession));
	MOCK_METHOD1(SendCustomResponse,
				void(dynamic jsonData));
	MOCK_METHOD1(SendHtmlResponse,
			void(std::string htmlContent));
	MOCK_METHOD3(SendImageResponse,
			void(const void* buffer, size_t contentLength, std::string contentType));
	MOCK_METHOD0(GetRequestedUrl,
			const std::string());
	MOCK_METHOD0(GetRequestType,
			const RequestType());
	MOCK_METHOD3(SendErrorResponse,
			void(std::string message, int statusCode, std::string statusMessage));


	// Fake these methods
	virtual void onRequest(std::unique_ptr<proxygen::HTTPMessage> request) noexcept override {}
	virtual void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override {}
	virtual void onEOM() noexcept override {}
	virtual void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override {}
	virtual void onError(proxygen::ProxygenError err) noexcept override {}
	virtual void requestComplete() noexcept override {}


};


class MockCefBrowser : public CefBrowser {
 public:
  MOCK_METHOD0(GetHost,
      CefRefPtr<CefBrowserHost>());
  MOCK_METHOD0(CanGoBack,
      bool());
  MOCK_METHOD0(GoBack,
      void());
  MOCK_METHOD0(CanGoForward,
      bool());
  MOCK_METHOD0(GoForward,
      void());
  MOCK_METHOD0(IsLoading,
      bool());
  MOCK_METHOD0(Reload,
      void());
  MOCK_METHOD0(ReloadIgnoreCache,
      void());
  MOCK_METHOD0(StopLoad,
      void());
  MOCK_METHOD0(GetIdentifier,
      int());
  MOCK_METHOD1(IsSame,
      bool(CefRefPtr<CefBrowser> that));
  MOCK_METHOD0(IsPopup,
      bool());
  MOCK_METHOD0(HasDocument,
      bool());
  MOCK_METHOD0(GetMainFrame,
      CefRefPtr<CefFrame>());
  MOCK_METHOD0(GetFocusedFrame,
      CefRefPtr<CefFrame>());
  MOCK_METHOD1(GetFrame,
      CefRefPtr<CefFrame>(int64 identifier));
  MOCK_METHOD1(GetFrame,
      CefRefPtr<CefFrame>(const CefString& name));
  MOCK_METHOD0(GetFrameCount,
      size_t());
  MOCK_METHOD1(GetFrameIdentifiers,
      void(std::vector<int64>& identifiers));
  MOCK_METHOD1(GetFrameNames,
      void(std::vector<CefString>& names));
  MOCK_METHOD2(SendProcessMessage,
      bool(CefProcessId target_process, CefRefPtr<CefProcessMessage> message));

  IMPLEMENT_REFCOUNTING(MockCefBrowser)
};


class MockCefBrowserHandler : public CefBrowserHandler {
 public:
  MOCK_METHOD0(GetRenderHandler,
      CefRefPtr<CefRenderHandler>());
  MOCK_METHOD2(GetViewRect,
      bool(CefRefPtr<CefBrowser> browser, CefRect& rect));
  MOCK_METHOD0(GetDisplayHandler,
      CefRefPtr<CefDisplayHandler>());
  MOCK_METHOD0(GetLifeSpanHandler,
      CefRefPtr<CefLifeSpanHandler>());
  MOCK_METHOD0(GetLoadHandler,
      CefRefPtr<CefLoadHandler>());
  MOCK_METHOD0(GetJSDialogHandler,
  	  CefRefPtr<CefJSDialogHandler>());
  MOCK_METHOD0(GetRequestHandler,
    	  CefRefPtr<CefRequestHandler>());
  MOCK_METHOD1(ResetBrowser,
      void(CefRefPtr<CefBrowser> browser));
  MOCK_METHOD2(StartBrowserSession,
      void(CefRefPtr<CefBrowser> browser, std::string url));
  MOCK_METHOD1(Initialize,
      void(BrowserPool* browserPool));
  MOCK_METHOD2(OnPageLoadExecuted,
      void(const CefString& string, int browserId));
  MOCK_METHOD2(setBrowserUrl,
      void(CefRefPtr<CefBrowser> browser, const CefString& url));
  MOCK_METHOD1(OnAfterCreated,
      void(CefRefPtr<CefBrowser> browser));
  MOCK_METHOD1(DoClose,
      bool(CefRefPtr<CefBrowser> browser));
  MOCK_METHOD1(OnBeforeClose,
      void(CefRefPtr<CefBrowser> browser));
  MOCK_METHOD4(OnLoadingStateChange,
      void(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward));
  MOCK_METHOD5(OnLoadError,
      void(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl));
  MOCK_METHOD3(OnLoadStart,
      void(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType type));
  MOCK_METHOD3(OnLoadEnd,
      void(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode));
  MOCK_METHOD6(OnPaint,
      void(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height));
  MOCK_METHOD3(OnProcessMessageReceived,
      bool(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message));
  MOCK_METHOD7(OnJSDialog,
		  bool(CefRefPtr<CefBrowser> browser, const CefString& origin_url, JSDialogType dialog_type, const CefString& message_text, const CefString& default_prompt_text, CefRefPtr<CefJSDialogCallback> callback, bool& suppress_message));
  MOCK_METHOD4(OnBeforeResourceLoad,
		  ReturnValue(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback));
  MOCK_METHOD4(OnResourceRedirect,
		  void(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefString& new_url));
  MOCK_METHOD4(OnResourceResponse,
		  bool(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefResponse> response));

  // Stop the compiler complaining because this implements some pure virtual functions that won't be used in the tests
  IMPLEMENT_REFCOUNTING(MockCefBrowserHandler)
};

#endif  /* ROXXY_BUILD */


