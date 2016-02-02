/*
 * Logging.cc
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#include "Logging.h"

std::string StringPrinter::print(const folly::IOBuf* buf) {
	std::string out;
	const uint8_t* data = buf->data();
	for (size_t i = 0; i < buf->length(); i++) {
		out += isprint(data[i]) ? data[i] : ' ';
	}
	return out;
}

