/*
 * SourceVisitor.cc
 *
 *  Created on: 12 Feb 2016
 *      Author: bam4d
 */

#include "SourceVisitor.h"
#include "CefBrowserHandler.h"

SourceVisitor::SourceVisitor(CefBrowserHandler* browserHandler, int browserId) {
	browserHandler_ = browserHandler;
	browserId_ = browserId;
}

SourceVisitor::~SourceVisitor() {
	// TODO Auto-generated destructor stub
}

// Called asynchronously when the HTML contents are available.
void SourceVisitor::Visit(const CefString& string) {

	LOG(INFO)<< "got page source";
	browserHandler_->OnSourceVisited(string, browserId_);
}
