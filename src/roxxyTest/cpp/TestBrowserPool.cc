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

#define NUM_BROWSERS 2
#define NUM_REQUESTERS 10
#define NUM_THREADS 10
TEST(BrowserPoolTest, TestAsyncAssignRelease) {

	CefRefPtr<MockCefBrowserHandler> handler = new MockCefBrowserHandler();
	BrowserPool *pool = new BrowserPool(handler, NUM_BROWSERS);
	std::vector<MockRenderProxyHandler*> renderProxyHandlers(NUM_REQUESTERS);
	std::vector<CefRefPtr<MockCefBrowser>> browsers(NUM_BROWSERS);

	std::vector<std::thread> threads(NUM_THREADS);

	for(int b = 0; b < NUM_BROWSERS; b++) {
		browsers[b] = new MockCefBrowser();
		EXPECT_CALL(*browsers[b].get(), GetIdentifier()).WillRepeatedly(Return(b));
		pool->RegisterBrowser(browsers[b]);
	}

	// Repeat the process of requesting browser to be assigned from the pool 10 times
	for(int t = 0; t < NUM_THREADS; t++) {
		threads[t] = std::thread([&] () {
			for(int reps = 0; reps < 3; reps++) {
				for(int r = 0; r < NUM_REQUESTERS; r++) {
					renderProxyHandlers[r] = new MockRenderProxyHandler();
					pool->AssignBrowserSync(renderProxyHandlers[r]);
					// do some sort of waiting here ?
					pool->ReleaseBrowserSync(renderProxyHandlers[r]);
				}
			}
		});
	}

	for (auto& t : threads) {
		t.join();
	}

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
