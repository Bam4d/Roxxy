/*
 * test_main.cc
 *
 *  Created on: 17 Mar 2016
 *      Author: bam4d
 */

#ifndef ROXXY_BUILD
#include <gtest/gtest.h>

// Build this if we are not doing a build of the main executable


using namespace testing;
int main(int argc, char* argv[]) {

	InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();

}

#endif  /* ROXXY_BUILD */


