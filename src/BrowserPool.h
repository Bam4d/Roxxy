/*
 * BrowserPool.h
 *
 *  Created on: 6 Feb 2016
 *      Author: bam4d
 */

#ifndef BROWSERPOOL_H_
#define BROWSERPOOL_H_

#include <folly/AtomicHashMap.h>
#include <list>

#include "include/wrapper/cef_helpers.h"

class RenderProxyHandler;
class CefBrowser;
class CefBrowserHandler;

using namespace std;

class BrowserPool {
public:
	BrowserPool(int browsers);
	virtual ~BrowserPool();

	// Gets a browser instance from the pool
	// This will block until a browser is available
	CefBrowser* AssignBrowserSync(RenderProxyHandler* renderProxyHandler);

	// Start the browser session with the assigned browser
	void StartBrowserSession(RenderProxyHandler* renderProxyHandler);

	// Send a response to the client
	void SendResponse(CefBrowser* browser, std::string responseContent);

	// Get the render proxy handler for a specific browser
	RenderProxyHandler* GetAssignedRenderProxyHandler(CefBrowser* browser);

	// Get the assigned browser for the renderProxyHandler
	CefBrowser* GetAssignedBrowser(RenderProxyHandler* renderProxyHandler);

	// Number of browsers in the pool
	int Size();

	void Initialize();

private:

	int numBrowsers_;

	// Keeps a map of handlers to their respective browsers
	folly::AtomicHashMap<int64_t, int64_t>* browserToHandler_;

	// Keeps a map of browsers to their repective handlers
	folly::AtomicHashMap<int64_t, int64_t>* handlerToBrowser_;

	CefRefPtr<CefBrowserHandler> browserHandler_;
};

#endif /* BROWSERPOOL_H_ */
