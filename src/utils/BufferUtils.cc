/*
 * bufferUtils.cc
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#include "BufferUtils.h"

std::string BufferUtils::getString(std::unique_ptr<folly::IOBuf> buf) {
	std::stringstream out;

	if (buf) {
		const folly::IOBuf* p = buf.get();
		do {
			out.write((const char*) p->data(), p->length());
			p = p->next();
		} while (p->next() != buf.get());

		return out.str();
	}
	return "";
}

dynamic BufferUtils::getJson(std::unique_ptr < folly::IOBuf > buf) {
	std::string jsonString = BufferUtils::getString(std::move(buf));
	return parseJson(jsonString);
}
