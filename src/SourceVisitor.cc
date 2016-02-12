/*
 * SourceVisitor.cc
 *
 *  Created on: 12 Feb 2016
 *      Author: bam4d
 */

#include "SourceVisitor.h"

SourceVisitor::SourceVisitor(CefRefPtr<CefBrowserHandler> browserHandler) {

	browserHandler_ = browserHandler;
}

SourceVisitor::~SourceVisitor() {
	// TODO Auto-generated destructor stub
}

// Called asynchronously when the HTML contents are available.
virtual void SourceVisitor::Visit(const CefString& string) {

}
