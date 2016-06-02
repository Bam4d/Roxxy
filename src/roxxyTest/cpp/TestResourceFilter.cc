/*
 * TestBrowserPool.cc
 *
 *  Created on: 02 Jun 2016
 *      Author: bam4d
 */

#include <gtest/gtest.h>
#include <vector>
#include <boost/regex.hpp>

#include "../../ResourceFilter.h"

TEST(ResourceFilterTest, TestFilter) {

	ResourceFilter filter;

	filter.AddRegexToFilter("https?://\\w+\\.adservice\\.com");

	EXPECT_EQ(filter.ShouldFilterUrl("http://test.adservice.com"), true);

	EXPECT_EQ(filter.ShouldFilterUrl("https://secure.adservice.com/v3/blah/script.js"), true);

	EXPECT_EQ(filter.ShouldFilterUrl("http://import.io"), false);

}

/**
 * Test that if the filter file has a bad regular expression that we just ignore it
 */
TEST(ResourceFilterTest, TestBadRegex) {

	ResourceFilter filter;
	filter.AddRegexToFilter("https?://\\w+\\.adse)rvice\\.com");

	EXPECT_EQ(filter.ShouldFilterUrl("http://test.adservice.com"), false);

	EXPECT_EQ(filter.ShouldFilterUrl("https://secure.adservice.com/v3/blah/script.js"), false);

	EXPECT_EQ(filter.ShouldFilterUrl("http://import.io"), false);

}
