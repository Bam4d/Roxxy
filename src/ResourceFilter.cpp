/*
 * ResourceFilter.cpp
 *
 *  Created on: 2 Jun 2016
 *      Author: bam4d
 */

#include<fstream>

#include "include/wrapper/cef_helpers.h"

#include "ResourceFilter.h"

ResourceFilter::ResourceFilter() {

}

ResourceFilter::~ResourceFilter() {

}

bool ResourceFilter::ShouldFilterUrl(std::string requestUrl) {
	for(boost::regex filterItem : filterItems_) {
		if(boost::regex_search(requestUrl, filterItem)) {
			return true;
		}
	}
	return false;
}

void ResourceFilter::LoadFilterList(std::string filename) {

	std::ifstream filterLoadStream(filename);

	if(!filterLoadStream) {
		LOG(WARNING)<<"Can not load resource filter list. Could not open file: "<<filename;
		return;
	}

	// Put the lines into the file
	std::string line;
	while(getline(filterLoadStream, line)) {
		AddRegexToFilter(line);
	}
}

void ResourceFilter::AddRegexToFilter(std::string expression) {

	try {
		filterItems_.push_back(boost::regex(expression));
	} catch(boost::regex_error e) {
		LOG(WARNING)<<"Regex \""<<expression<<"\" could not be added: "<<e.what();
	}
}
