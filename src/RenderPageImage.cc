/*
 * RenderPNG.cpp
 *
 *  Created on: 10 Mar 2016
 *      Author: bam4d
 */

#include <png.h>
#include <stdio.h>
#include <algorithm>
#include "include/base/cef_logging.h"
#include "RenderPageImage.h"


static void png_to_buffer(png_structp png_ptr, png_bytep data, png_size_t length) {
	/* with libpng15 next line causes pointer deference error; use libpng12 */
	struct png_buffer* p = (struct png_buffer*) png_get_io_ptr(png_ptr);
	size_t nsize = p->size + length;

	// Reuse buffer if we have adequate size in the buffer, otherwise allocate it or grow it
	if(p->buffer && p->size < nsize)
		p->buffer = realloc(p->buffer, nsize);
	else
		p->buffer = malloc(nsize);


	if(!p->buffer)
	png_error(png_ptr, "Write Error");

	/* copy new bytes to end of buffer */
	memcpy(p->buffer + p->size, data, length);
	p->size += length;
}


RenderPageImage::RenderPageImage() {
	// TODO Auto-generated constructor stub

}

RenderPageImage::~RenderPageImage() {
	// TODO Auto-generated destructor stub
}



/**
 * |buffer| will be |width|*|height|*4 bytes in size and represents a
 * BGRA image with an upper-left origin.
 */
void RenderPageImage::RenderPNG(const void* bgraBuffer, png_buffer* pngBuffer, size_t width, size_t height) {
	DCHECK(pngBuffer != nullptr);
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	size_t x, y;
	png_uint_32 bytes_per_row;
	png_bytep *row_pointers = NULL;

	/* Initialize the write struct. */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		return;
	}

	/* Initialize the info struct. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_write_struct(&png_ptr, NULL);
		return;
	}

	/* Set up error handling. */
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return;
	}

	/* Set image attributes. */
	png_set_IHDR(png_ptr, info_ptr, width, height, 8,
	PNG_COLOR_TYPE_RGBA,
	PNG_INTERLACE_NONE,
	PNG_COMPRESSION_TYPE_DEFAULT,
	PNG_FILTER_TYPE_DEFAULT);

	/* Initialize rows of PNG. */
	bytes_per_row = width * 4;
	row_pointers = (png_bytep*) png_malloc(png_ptr,
			height * sizeof(png_byte *));
	for (y = 0; y < height; ++y) {
		png_byte* row = (png_byte*) malloc(png_get_rowbytes(png_ptr, info_ptr));
		row_pointers[y] = row;
		png_byte* bgraPixel = (png_byte*) bgraBuffer + y * bytes_per_row;
		for (x = 0; x < width; ++x) {
			*row++ = *(bgraPixel + 2);
			*row++ = *(bgraPixel + 1);
			*row++ = *bgraPixel;
			*row++ = *(bgraPixel + 3);

			bgraPixel += 4;
		}
	}

	// Reset the png buffer to the start, so we overwrite it
	pngBuffer->size = 0;

	png_set_write_fn(png_ptr, pngBuffer, png_to_buffer, NULL);

	/* Actually write the image data. */
	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	/* Cleanup. */
	for (y = 0; y < height; y++) {
		png_free(png_ptr, row_pointers[y]);
	}
	png_free(png_ptr, row_pointers);

	/* Finish writing. */
	png_destroy_write_struct(&png_ptr, &info_ptr);
}
