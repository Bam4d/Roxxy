/*
 * V8CallbackHandler.h
 *
 *  Created on: 26 Feb 2016
 *      Author: bam4d
 */

#ifndef V8CALLBACKHANDLER_H_
#define V8CALLBACKHANDLER_H_

#include "include/cef_v8.h"

class V8CallbackHandler: public CefV8Handler {
public:
	V8CallbackHandler();
	virtual ~V8CallbackHandler();

	bool Execute(const CefString& name,
	                       CefRefPtr<CefV8Value> object,
	                       const CefV8ValueList& arguments,
	                       CefRefPtr<CefV8Value>& retval,
	                       CefString& exception) override;

private:
	IMPLEMENT_REFCOUNTING(V8CallbackHandler);
};

#endif /* V8CALLBACKHANDLER_H_ */
