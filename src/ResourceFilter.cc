/*
 * ResourceFilter.cpp
 *
 *  Created on: 2 Jun 2016
 *      Author: bam4d
 */

#include <sstream>

#include "include/wrapper/cef_helpers.h"
#include "AdBlockPlusFilter/ABPFilterParser.h"

#include "ResourceFilter.h"

using namespace std;

ResourceFilter::ResourceFilter() {
	parser = new ABPFilterParser();
}

ResourceFilter::~ResourceFilter() {
	delete parser;
}

bool ResourceFilter::ShouldFilterUrl(std::string requestUrl, std::string currentDomain) {
	return parser->matches(requestUrl.c_str(), FilterOption::FOResourcesOnly, currentDomain.c_str());
}

void ResourceFilter::LoadFilters(std::string foldername) {
	LOG(INFO)<<"Loading filters from directory: "<<foldername;
	try {
		path p(foldername);
		for (auto i = directory_iterator(p); i != directory_iterator(); i++)
		{
			if (!is_directory(i->path())) //we eliminate directories
			{
				LOG(INFO)<<"Filter file: "<<i->path().filename().string();
				std::string filterFile = getFileContents(i->path());
				parser->parse(filterFile.c_str());
			}
			else
				continue;
		}
	} catch(...) {
		LOG(WARNING)<<"Cannot load filters from directory: "<<foldername;
	}
}

std::string ResourceFilter::getFileContents(path filename)
{
  boost::filesystem::ifstream fileStream;

  fileStream.open(filename, std::ios::in);

  if (!fileStream) {
	  LOG(WARNING)<<"Cannot load filter file";
	  return "";
  }

  stringstream ss;

  ss << fileStream.rdbuf();

  return ss.str();
}

