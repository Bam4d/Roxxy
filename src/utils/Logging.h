/*
 * Logging.h
 *
 *  Created on: 26 Jan 2016
 *      Author: bam4d
 */

#ifndef UTILS_LOGGING_H_
#define UTILS_LOGGING_H_

#include <folly/io/IOBuf.h>

class StringPrinter {
public:
  static std::string print(const folly::IOBuf* buf);
};

#endif /* UTILS_LOGGING_H_ */
