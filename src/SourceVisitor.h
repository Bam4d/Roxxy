/*
 * SourceVisitor.h
 *
 *  Created on: 12 Feb 2016
 *      Author: bam4d
 */

#ifndef SOURCEVISITOR_H_
#define SOURCEVISITOR_H_

#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

class SourceVisitor: public CefStringVisitor {
public:
	SourceVisitor(CefRefPtr<CefBrowserHandler> browserHandler);
	virtual ~SourceVisitor();

	CefRefPtr<CefBrowserHandler> browserHandler_;

	IMPLEMENT_REFCOUNTING (SourceVisitor);
};

#endif /* SOURCEVISITOR_H_ */
