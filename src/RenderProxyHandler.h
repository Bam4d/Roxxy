/*
 * RenderProxyHandler.h
 *
 *  Created on: 17 Mar 2016
 *      Author: bam4d
 */

#ifndef SRC_RENDERPROXYHANDLER_H_
#define SRC_RENDERPROXYHANDLER_H_

#include <proxygen/httpserver/RequestHandler.h>
#include <folly/json.h>


enum RequestType: uint8_t {
	HTML,
	PNG,
	CUSTOM,
	STATUS,
};

class png_buffer;

using folly::dynamic;

class RenderProxyHandler: public proxygen::RequestHandler {
public:
	virtual ~RenderProxyHandler() {};

	virtual void PageRenderCompleted(const std::string htmlContent, int status, png_buffer* pngBuffer) = 0;

	virtual const std::string GetRequestedUrl() = 0;

	virtual const RequestType GetRequestType() = 0;
private:
	virtual void SendHtmlResponse(std::string responseData) = 0;

	virtual void SendImageResponse(const void* buffer, size_t contentLength, std::string contentType) = 0;

	virtual void SendCustomResponse(dynamic jsonResponse) = 0;

};


#endif /* SRC_RENDERPROXYHANDLER_H_ */
