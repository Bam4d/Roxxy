/*
 * RenderPNG.h
 *
 *  Created on: 10 Mar 2016
 *      Author: bam4d
 */

#ifndef SRC_RENDERPNG_H_
#define SRC_RENDERPNG_H_

#include <string>

class RenderPNG {
public:
	RenderPNG();
	virtual ~RenderPNG();

	static void Render(std::string name, const void* bgraBuffer, int width, int height);
};

#endif /* SRC_RENDERPNG_H_ */
