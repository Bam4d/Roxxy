/*
 * RoxxyRenderApp.h
 *
 *  Created on: 7 Mar 2016
 *      Author: bam4d
 */

#ifndef SRC_ROXXYRENDERAPP_H_
#define SRC_ROXXYRENDERAPP_H_

#include "include/cef_app.h"

class RoxxyRenderApp : public CefApp, CefRenderProcessHandler {
public:
	RoxxyRenderApp();
	virtual ~RoxxyRenderApp();

	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
		return this;
	}

	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
		                                CefRefPtr<CefFrame> frame,
		                                CefRefPtr<CefV8Context> context) override;

	// Render process message recieved
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
											CefProcessId source_process,
											CefRefPtr<CefProcessMessage> message) override;

	virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) override {
	}

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(RoxxyRenderApp)
};

#endif /* SRC_ROXXYRENDERAPP_H_ */
