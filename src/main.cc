/*
 * CefBrowserHandler.cpp
 *
 *  Created on: 2 Feb 2016
 *      Author: bam4d
 */

#ifdef ROXXY_BUILD
#include "Roxxy.h"

// Only build this main method if we are not building a test module

// Entry point function for all processes.
int main(int argc, char* argv[]) {

	Roxxy r;

	return r.Run(argc, argv);

}

#endif /* GTEST */
