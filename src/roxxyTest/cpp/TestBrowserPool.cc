/*
 * TestBrowserPool.cc
 *
 *  Created on: 17 Mar 2016
 *      Author: bam4d
 */

#ifndef ROXXY_BUILD

#include <gtest/gtest.h>
#include <vector>
#include <thread>

#include "Mocks.cc"

TEST(BrowserPoolTest, TestGenerateBrowsers) {

	CefRefPtr<MockCefBrowserHandler> handler = new MockCefBrowserHandler();
	BrowserPool *pool = new BrowserPool(handler, 10);

	EXPECT_EQ(pool->Size(), 10);
	EXPECT_CALL(*handler.get(), Initialize(pool))
			.Times(1);

	pool->Initialize();

	delete pool;
}

/**
 * Test that we can register browsers with the browser pool and retieve them by their unique ID
 */
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

/**
 * Test that we can assign browsers to proxy handlers and release them safely, the assignment should also be consistent
 */
TEST(BrowserPoolTest, TestAssignBrowser) {
	CefRefPtr<MockCefBrowserHandler> handler = new MockCefBrowserHandler();
	BrowserPool *pool = new BrowserPool(handler, 10);
	MockRenderProxyHandler *renderProxyHandler1 = new MockRenderProxyHandler();
	MockRenderProxyHandler *renderProxyHandler2 = new MockRenderProxyHandler();
	MockRenderProxyHandler *renderProxyHandler3 = new MockRenderProxyHandler();

	CefRefPtr<MockCefBrowser> browser1 = new MockCefBrowser();
	CefRefPtr<MockCefBrowser> browser2 = new MockCefBrowser();
	CefRefPtr<MockCefBrowser> browser3 = new MockCefBrowser();

	EXPECT_CALL(*browser1.get(), GetIdentifier()).WillRepeatedly(Return(1));
	EXPECT_CALL(*browser2.get(), GetIdentifier()).WillRepeatedly(Return(2));
	EXPECT_CALL(*browser3.get(), GetIdentifier()).WillRepeatedly(Return(3));

	pool->Initialize();
	pool->RegisterBrowser(browser1);
	pool->RegisterBrowser(browser2);
	pool->RegisterBrowser(browser3);

	EXPECT_EQ(pool->getBrowserById(1).get(), browser1);
	EXPECT_EQ(pool->getBrowserById(2).get(), browser2);
	EXPECT_EQ(pool->getBrowserById(3).get(), browser3);

	pool->AssignBrowserSync(renderProxyHandler1);
	pool->AssignBrowserSync(renderProxyHandler2);
	pool->AssignBrowserSync(renderProxyHandler3);

	// Should assign in the same order they were registered
	EXPECT_EQ(pool->GetAssignedRenderProxyHandler(1), renderProxyHandler1);
	EXPECT_EQ(pool->GetAssignedBrowserId(renderProxyHandler1), 1);
	EXPECT_EQ(pool->GetAssignedRenderProxyHandler(2), renderProxyHandler2);
	EXPECT_EQ(pool->GetAssignedBrowserId(renderProxyHandler2), 2);
	EXPECT_EQ(pool->GetAssignedRenderProxyHandler(3), renderProxyHandler3);
	EXPECT_EQ(pool->GetAssignedBrowserId(renderProxyHandler3), 3);

	// Now we release browsers and check
	EXPECT_CALL(*handler.get(), ResetBrowser(_)).Times(3);

	pool->ReleaseBrowserSync(renderProxyHandler1);
	pool->ReleaseBrowserSync(renderProxyHandler2);
	pool->ReleaseBrowserSync(renderProxyHandler3);

	delete pool;
}

/**
 * Test that if we have NUM_BROWSERS browsers and there are NUM_THREADS users simultaneously doing NUM_REQUESTERS requests,
 * we can easily track the browsers that are currently being used and which are not, we should also have no race conditions or mis-assigned browser/proxyhandlers
 */
#define NUM_BROWSERS 50
#define NUM_REQUESTERS 10
#define NUM_THREADS 100
TEST(BrowserPoolTest, TestAsyncAssignRelease) {

	MockCefBrowserHandler* handler = new MockCefBrowserHandler();
	BrowserPool *pool = new BrowserPool(CefRefPtr<MockCefBrowserHandler>(handler), NUM_BROWSERS);
	std::vector<MockCefBrowser*> browsers(NUM_BROWSERS);

	std::vector<std::thread> threads(NUM_THREADS);

	EXPECT_CALL(*handler, ResetBrowser(_)).Times(NUM_REQUESTERS*NUM_THREADS);

	for(int b = 0; b < NUM_BROWSERS; b++) {
		browsers[b] = new MockCefBrowser();
		EXPECT_CALL(*browsers[b], GetIdentifier()).WillRepeatedly(Return(b));
		pool->RegisterBrowser(CefRefPtr<MockCefBrowser>(browsers[b]));
	}

	// Repeat the process of requesting browser to be assigned from the pool 10 times
	for(int t = 0; t < NUM_THREADS; t++) {
		threads[t] = std::thread([&] () {
			for(int r = 0; r < NUM_REQUESTERS; r++) {
				MockRenderProxyHandler* renderProxyHandler = new MockRenderProxyHandler();
				int browserId = pool->AssignBrowserSync(renderProxyHandler);
				EXPECT_TRUE(browserId<NUM_BROWSERS);
				EXPECT_EQ(pool->GetAssignedBrowserId(renderProxyHandler), browserId);
				EXPECT_EQ(pool->GetAssignedRenderProxyHandler(browserId), renderProxyHandler);

				pool->ReleaseBrowserSync(renderProxyHandler);

				delete renderProxyHandler;
			}
		});
	}

	for (auto& t : threads) {
		t.join();
	}

	for(auto brow : browsers) {
		delete brow;
	}
	delete handler;

}


/**
 * Test that if we try to get an assigned browser that is not in the pool, there is a death signal
 */
TEST(BrowserPoolDeathTest, GetAssignedBrowserId) {
	CefRefPtr<MockCefBrowserHandler> handler = new MockCefBrowserHandler();
	BrowserPool *pool = new BrowserPool(handler, 10);
	MockRenderProxyHandler *renderProxyHandler1 = new MockRenderProxyHandler();
	pool->Initialize();

	EXPECT_EXIT(pool->GetAssignedBrowserId(renderProxyHandler1), ::testing::KilledBySignal(SIGABRT), "");
}

/**
 * Test that if we try to get an assigned proxy handler for a browser that is not in the pool, there is a death signal
 */
TEST(BrowserPoolDeathTest, GetAssignedRenderProxyHandler) {
	CefRefPtr<MockCefBrowserHandler> handler = new MockCefBrowserHandler();
	BrowserPool *pool = new BrowserPool(handler, 10);
	CefRefPtr<MockCefBrowser> browser1 = new MockCefBrowser();

	EXPECT_CALL(*browser1.get(), GetIdentifier()).WillRepeatedly(Return(1));

	pool->Initialize();

	EXPECT_EXIT(pool->GetAssignedRenderProxyHandler(browser1->GetIdentifier()), ::testing::KilledBySignal(SIGABRT), "");
}

#endif  /* ROXXY_BUILD */
