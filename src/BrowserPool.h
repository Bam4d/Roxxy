/*
 * BrowserPool.h
 *
 *  Created on: 6 Feb 2016
 *      Author: bam4d
 */

#ifndef BROWSERPOOL_H_
#define BROWSERPOOL_H_

#include <list>
#include <map>
#include <mutex>

#include "include/wrapper/cef_helpers.h"
#include "folly/MPMCQueue.h"

#include "RenderPageImage.h"

class RenderProxyHandler;
class CefBrowserHandler;
class CefBrowser;

using namespace std;
using namespace folly;

/**
 * Structure containing session information for a browser.
 *
 * For example, what has been requested on the API, and what the current state of the browser is
 */
struct BrowserSession {

	/**
	 * If the browser has loaded or is still loading
	 */
	bool isLoaded = false;

	/**
	 * Scripts to run on the page after load if any
	 */
	std::map<std::string, std::string> scripts;

	/**
	 * Status code of the page (default 200)
	 */
	int statusCode = 200;

	/**
	 * Html content of the page
	 */
	std::string htmlContent;

	/**
	 * Buffer to store a png image of the page, if requested
	 */
	png_buffer pngBuffer;

	/**
	 * Whether or not the image of the page has been requested
	 */
	bool pngRequested;

	/**
	 * The final url of the web page, will be different if there are redirects
	 */
	std::string pageUrl;


	/**
	 * The domain of the pageUrl
	 */
	std::string pageDomain;
};

/**
 * Handles routing of requests from API users to assigned browsers
 */
class BrowserPool {
public:
	BrowserPool(CefRefPtr<CefBrowserHandler> cefBrowserHandler, int browsers);
	virtual ~BrowserPool();

	void RegisterBrowser(CefRefPtr<CefBrowser> browser);

	int AssignBrowserSync(RenderProxyHandler* renderProxyHandler);

	void ReleaseBrowserSync(RenderProxyHandler* renderProxyHandler);

	void StartBrowserSession(RenderProxyHandler* renderProxyHandler);

	// Get the render proxy handler for a specific browser
	RenderProxyHandler* GetAssignedRenderProxyHandler(int browserId);

	// Get the assigned browser for the renderProxyHandler
	int GetAssignedBrowserId(RenderProxyHandler* renderProxyHandler);

	// Number of browsers in the pool
	int Size();

	void Initialize();

	CefRefPtr<CefBrowser> getBrowserById(int id);

	BrowserSession* GetBrowserSessionById(int id);

private:

	std::mutex browser_routing_mutex;

	int numBrowsers_;

	//Load start and end counters for each browser
	std::map<int, BrowserSession> browserSession_;

	// Manage the browser resources
	typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
	BrowserList browserList_;

	/**
	 * Queue of browsers that are currently un-assigned
	 */
	MPMCQueue<int>* freeBrowserList_;

	/**
	 * Keeps a map of handlers to their respective browsers
	 */
	std::map<int, RenderProxyHandler*> browserIdToHandler_;

	/**
	 * Keeps a map of browsers to their repective handlers
	 */
	std::map<RenderProxyHandler*, int> handlerToBrowserId_;

	/**
	 * A browser handler, used to communicate messages to the browsers
	 */
	CefRefPtr<CefBrowserHandler> browserHandler_;
};

#endif /* BROWSERPOOL_H_ */
