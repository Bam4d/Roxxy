/*
 * SourceVisitor.h
 *
 *  Created on: 12 Feb 2016
 *      Author: bam4d
 */

#ifndef SOURCEVISITOR_H_
#define SOURCEVISITOR_H_

#include "include/cef_base.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "include/cef_string_visitor.h"


// Forward declaration
class CefBrowserHandler;

class SourceVisitor: public CefStringVisitor {
public:

	SourceVisitor(CefBrowserHandler* browserHandler, int browserId);
	virtual ~SourceVisitor();

	void Visit(const CefString& string) override;

private:
	int browserId_;
	CefBrowserHandler* browserHandler_;


	IMPLEMENT_REFCOUNTING(SourceVisitor)
};

#endif /* SOURCEVISITOR_H_ */
