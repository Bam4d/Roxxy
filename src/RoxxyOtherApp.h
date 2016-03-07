/*
 * RoxxyOtherApp.h
 *
 *  Created on: 7 Mar 2016
 *      Author: bam4d
 */

#ifndef SRC_ROXXYOTHERAPP_H_
#define SRC_ROXXYOTHERAPP_H_

#include "include/cef_app.h"

class RoxxyOtherApp : public CefApp {
public:
	RoxxyOtherApp();
	virtual ~RoxxyOtherApp();

private:
	IMPLEMENT_REFCOUNTING(RoxxyOtherApp)
};

#endif /* SRC_ROXXYOTHERAPP_H_ */
