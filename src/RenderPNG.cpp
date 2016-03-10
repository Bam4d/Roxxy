/*
 * RenderPNG.cpp
 *
 *  Created on: 10 Mar 2016
 *      Author: bam4d
 */


#include <png.h>
#include <stdio.h>
#include "RenderPNG.h"
#include "include/base/cef_logging.h"

RenderPNG::RenderPNG() {
	// TODO Auto-generated constructor stub

}

RenderPNG::~RenderPNG() {
	// TODO Auto-generated destructor stub
}

/**
 * |buffer| will be |width|*|height|*4 bytes in size and represents a
 * BGRA image with an upper-left origin.
 */
void RenderPNG::Render(std::string name, const void* bgraBuffer, int width, int height) {
	FILE *fp = fopen((name+".png").c_str(), "wb");
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	size_t x, y;
	png_uint_32 bytes_per_row;
	png_bytep *row_pointers = NULL;

	LOG(INFO)<< "W"<<width<<" H"<<height;

	if (fp == NULL) return;

	/* Initialize the write struct. */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fclose(fp);
		return;
	}

	/* Initialize the info struct. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_write_struct(&png_ptr, NULL);
		fclose(fp);
		return;
	}

	/* Set up error handling. */
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
		return;
	}

	/* Set image attributes. */
	png_set_IHDR(png_ptr,
				 info_ptr,
				 width,
				 height,
				 8,
				 PNG_COLOR_TYPE_RGBA,
				 PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_DEFAULT,
				 PNG_FILTER_TYPE_DEFAULT);

	LOG(INFO)<< "building png";
	/* Initialize rows of PNG. */
	bytes_per_row = width * 4;
	row_pointers = (png_bytep*)png_malloc(png_ptr, height * sizeof(png_byte *));
	for (y = 0; y < height; ++y) {
		png_byte* row = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));
		row_pointers[y] = row;
		png_byte* bgraPixel = (png_byte*)bgraBuffer;//+y*bytes_per_row;
		for (x = 0; x < width; ++x) {
			//LOG(INFO)<< "W"<<x<<" H"<<y;
			*row = *(bgraPixel+2);
			*(row+1) = *(bgraPixel+1);
			*(row+2) = *bgraPixel;
			*(row+3) = *(bgraPixel+3);

			row += 4;
			bgraPixel += 4;
		}
	}

	/* Actually write the image data. */
	png_init_io(png_ptr, fp);
	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	/* Cleanup. */
	for (y = 0; y < height; y++) {
		png_free(png_ptr, row_pointers[y]);
	}
	png_free(png_ptr, row_pointers);

	/* Finish writing. */
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);
}
