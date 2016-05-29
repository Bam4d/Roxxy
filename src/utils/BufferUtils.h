/*
 * bufferUtils.h
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#ifndef UTILS_BUFFERUTILS_H_
#define UTILS_BUFFERUTILS_H_

#include <folly/json.h>
#include <folly/io/IOBuf.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

using folly::dynamic;
using folly::parseJson;
using folly::toJson;

/**
 * Utility functions for folly/proxygen buffers
 */
class BufferUtils {
public:
	static std::string GetString(std::unique_ptr<folly::IOBuf> buf);
	static dynamic GetJson(std::unique_ptr<folly::IOBuf> buf);

	static std::string Base64Encode(const char* buffer, size_t length);
	static std::string Base64Decode(const std::string &input);
};

#endif /* UTILS_BUFFERUTILS_H_ */
