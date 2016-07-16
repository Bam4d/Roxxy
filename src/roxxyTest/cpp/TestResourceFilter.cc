/*
 * TestBrowserPool.cc
 *
 *  Created on: 02 Jun 2016
 *      Author: bam4d
 */

#ifndef ROXXY_BUILD

#include <gtest/gtest.h>
#include <vector>
#include <unistd.h>
#include <limits.h>

#include "boost/filesystem/operations.hpp"

#include "../../ResourceFilter.h"

TEST(ResourceFilterTest, TestFilter) {

	ResourceFilter filter;

	char buff[PATH_MAX];
	ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
	if (len != -1) {
	  buff[len] = '\0';
	  std::string current_dir(buff);
	  current_dir = path(current_dir).remove_filename().string();
	  filter.LoadFilters(current_dir+"/filters");
	}

	EXPECT_EQ(filter.ShouldFilterUrl("http://test.adservice.com/thing.swf?1&clicktag=ABhe8576", "google.com"), true);

	EXPECT_EQ(filter.ShouldFilterUrl("http://test.adservice.com/antiadblockmsg.html", "google.com"), true);

	EXPECT_EQ(filter.ShouldFilterUrl("http://import.io", "google.com"), false);

	EXPECT_EQ(filter.ShouldFilterUrl("http://google.com", "google.com"), false);

}

#endif  /* ROXXY_BUILD */

