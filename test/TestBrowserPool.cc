/*
 * TestBrowserPool.cc
 *
 *  Created on: 17 Mar 2016
 *      Author: bam4d
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/BrowserPool.h"
#include "../src/CefBrowserHandler.h"
#include "../src/RenderProxyHandler.h"

using namespace testing;

class MockRenderProxyHandler : public RenderProxyHandler {
public:
	MOCK_METHOD1(SendResponse,
			void(std::string response_data));
	MOCK_METHOD3(SendImageResponse,
			void(const void* buffer, size_t contentLength, std::string contentType));
	MOCK_METHOD0(GetRequestedUrl,
			const std::string());
	MOCK_METHOD0(GetRequestType,
			const RequestType());

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
      void(CefRefPtr<CefBrowser> browser, 			bool isLoading, 			bool canGoBack, 			bool canGoForward));
  MOCK_METHOD5(OnLoadError,
      void(CefRefPtr<CefBrowser> browser, 			CefRefPtr<CefFrame> frame, ErrorCode errorCode, 	const CefString& errorText, const CefString& failedUrl));
  MOCK_METHOD2(OnLoadStart,
      void(CefRefPtr<CefBrowser> browser, 			CefRefPtr<CefFrame> frame));
  MOCK_METHOD3(OnLoadEnd,
      void(CefRefPtr<CefBrowser> browser, 			CefRefPtr<CefFrame> frame, int httpStatusCode));
  MOCK_METHOD6(OnPaint,
      void(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height));
  MOCK_METHOD3(OnProcessMessageReceived,
      bool(CefRefPtr<CefBrowser> browser, 	 CefProcessId source_process, 	 CefRefPtr<CefProcessMessage> message));

  // Stop the compiler complaining because this implements some pure virtual functions that won't be used in the tests
  IMPLEMENT_REFCOUNTING(MockCefBrowserHandler)
};

TEST(BrowserPoolTest, TestGenerateBrowsers) {

	CefRefPtr<MockCefBrowserHandler> handler = new MockCefBrowserHandler();
	BrowserPool *pool = new BrowserPool(handler, 10);

	EXPECT_EQ(pool->Size(), 10);
	EXPECT_CALL(*handler.get(), Initialize(pool))
			.Times(1);

	pool->Initialize();

	delete pool;
}

TEST(BrowserPoolTest, TestRegisterBrowser) {
	CefRefPtr<CefBrowserHandler> handler = new MockCefBrowserHandler();
	BrowserPool *pool = new BrowserPool(handler, 10);

	CefRefPtr<MockCefBrowser> browser = new MockCefBrowser();

	EXPECT_CALL(*browser.get(), GetIdentifier()).WillRepeatedly(Return(1));

	pool->Initialize();
	pool->RegisterBrowser(browser);

	EXPECT_EQ(pool->getBrowserById(1).get(), browser);

	delete pool;
}

TEST(BrowserPoolTest, TestAssignBrowser) {
	CefRefPtr<MockCefBrowserHandler> handler = new MockCefBrowserHandler();
	BrowserPool *pool = new BrowserPool(handler, 10);
	RenderProxyHandler *renderProxyHandler = new RenderProxyHandler();

	CefRefPtr<MockCefBrowser> browser = new MockCefBrowser();

	EXPECT_CALL(*browser.get(), GetIdentifier()).WillRepeatedly(Return(1));

	pool->Initialize();
	pool->RegisterBrowser(browser);
	pool->AssignBrowserSync(renderProxyHandler);

	EXPECT_EQ(pool->getBrowserById(1).get(), browser);

	delete pool;
}
//
//TEST(BrowserPoolTest, TestReleaseBrowser) {
//	CefRefPtr<CefBrowserHandler> handler = new MockCefBrowserHandler();
//	BrowserPool *pool = new BrowserPool(handler, 10);
//
//	CefRefPtr<CefBrowser> browser = new MockCefBrowser();
//
//	pool->RegisterBrowser(browser);
//}

