/*
 * RenderPNG.h
 *
 *  Created on: 10 Mar 2016
 *      Author: bam4d
 */

#ifndef SRC_RENDERPAGEIMAGE_H_
#define SRC_RENDERPAGEIMAGE_H_

#include <string>

/* structure to store PNG image bytes */
struct png_buffer
{
  void *buffer = nullptr;
  size_t size = 0;
};

class RenderPageImage {
public:
	RenderPageImage();
	virtual ~RenderPageImage();

	static void RenderPNG(const void* bgraBuffer, png_buffer* pngBuffer, size_t width, size_t height);
};

#endif /* SRC_RENDERPAGEIMAGE_H_ */
