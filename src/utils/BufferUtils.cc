/*
 * bufferUtils.cc
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#include "BufferUtils.h"

std::string BufferUtils::GetString(std::unique_ptr<folly::IOBuf> buf) {

	if(buf){
		return std::string(reinterpret_cast<const char*>(buf->data()),
	                           buf->length());
	}

	return "";
}

dynamic BufferUtils::GetJson(std::unique_ptr < folly::IOBuf > buf) {
	std::string jsonString = BufferUtils::GetString(std::move(buf));
	return parseJson(jsonString);
}
