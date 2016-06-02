/*
 * DomainBlacklist.h
 *
 *  Created on: 2 Jun 2016
 *      Author: bam4d
 */

#ifndef RESOURCEFILTER_H_
#define RESOURCEFILTER_H_

#include<boost/regex.hpp>
#include<vector>

class ResourceFilter {
public:
	ResourceFilter();
	virtual ~ResourceFilter();
	
	/**
	 * If the url is accepted by the filter
	 */
	bool ShouldFilterUrl(std::string requestUrl);

	/**
	 * Load a filter definition file
	 *
	 * The filter deinition is line seperated list of regular expressions used to filter resources such as
	 * adverts and trackers
	 */
	void LoadFilterList(std::string filename);
	
	/**
	 * Add a regular expression to the resource filter
	 */
	void AddRegexToFilter(std::string filter);

private:
	std::vector<boost::regex> filterItems_;
};

#endif /* RESOURCEFILTER_H_ */
