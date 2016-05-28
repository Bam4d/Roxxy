/*
 * RenderProxyHandler.h
 *
 *  Created on: 17 Mar 2016
 *      Author: bam4d
 */

#ifndef SRC_RENDERPROXYHANDLER_H_
#define SRC_RENDERPROXYHANDLER_H_

#include <proxygen/httpserver/RequestHandler.h>

enum RequestType: uint8_t {
	HTML,
	PNG,
	CUSTOM,
	STATUS,
};

class RenderProxyHandler: public proxygen::RequestHandler {
public:
	virtual ~RenderProxyHandler() {};

	virtual void SendResponse(std::string response_data) = 0;

	virtual void SendImageResponse(const void* buffer, size_t contentLength, std::string contentType) = 0;

	virtual const std::string GetRequestedUrl() = 0;

	virtual const RequestType GetRequestType() = 0;

};


#endif /* SRC_RENDERPROXYHANDLER_H_ */
