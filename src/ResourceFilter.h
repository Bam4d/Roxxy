/*
 * DomainBlacklist.h
 *
 *  Created on: 2 Jun 2016
 *      Author: bam4d
 */

#ifndef RESOURCEFILTER_H_
#define RESOURCEFILTER_H_

#include<vector>

#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"
using namespace boost::filesystem;

class ABPFilterParser;

class ResourceFilter {
public:
	ResourceFilter();
	virtual ~ResourceFilter();

	/**
	 * If the url is accepted by the filter
	 */
	bool ShouldFilterUrl(std::string requestUrl, std::string currentDomain);

	/**
	 * Load filters
	 * Load adblockplus style filters from a folder such as easylist etc.
	 */
	void LoadFilters(std::string foldername);

private:
	std::string getFileContents(path filename);

	// AdBlockPlus filter
	ABPFilterParser *parser;
};

#endif /* RESOURCEFILTER_H_ */
