/*
 * Copyright 2006 Richard Wilson <info@tinct.net>
 * Copyright 2008 Sean Fox <dyntryx@gmail.com>
 *
 * This file is part of NetSurf, http://www.netsurf-browser.org/
 *
 * NetSurf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * NetSurf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** \file
 * BMP file decoding (interface).
 */

#ifndef _NETSURF_IMAGE_BMPREAD_H_
#define _NETSURF_IMAGE_BMPREAD_H_

#include <stdbool.h>

/* bmp flags */
#define BMP_NEW		0
#define BMP_OPAQUE		(1 << 0)	/** image is opaque */
#define BMP_CLEAR_MEMORY	(1 << 1)	/** memory should be wiped */

/* error return values */
typedef enum {
	BMP_OK = 0,
	BMP_INSUFFICIENT_MEMORY = 1,
	BMP_INSUFFICIENT_DATA = 2,
	BMP_DATA_ERROR = 3
} bmp_result;

/* encoding types */
typedef enum {
  	BMP_ENCODING_RGB = 0,
  	BMP_ENCODING_RLE8 = 1,
  	BMP_ENCODING_RLE4 = 2,
  	BMP_ENCODING_BITFIELDS = 3
} bmp_encoding;

/*	API for Bitmap callbacks
*/
typedef void* (*bmp_bitmap_cb_create)(int width, int height, unsigned int state);
typedef void (*bmp_bitmap_cb_destroy)(void *bitmap);
typedef void (*bmp_bitmap_cb_set_suspendable)(void *bitmap, void *private_word,
		void (*invalidate)(void *bitmap, void *private_word));
typedef unsigned char* (*bmp_bitmap_cb_get_buffer)(void *bitmap);
typedef size_t (*bmp_bitmap_cb_get_bpp)(void *bitmap);

/*	The Bitmap callbacks function table
*/
typedef struct bmp_bitmap_callback_vt_s {
	bmp_bitmap_cb_create bitmap_create;			/**< Create a bitmap. */
	bmp_bitmap_cb_destroy bitmap_destroy;			/**< Free a bitmap. */
	bmp_bitmap_cb_set_suspendable bitmap_set_suspendable;	/**< The bitmap image can be suspended. */
	bmp_bitmap_cb_get_buffer bitmap_get_buffer;		/**< Return a pointer to the pixel data in a bitmap. */
	bmp_bitmap_cb_get_bpp bitmap_get_bpp;			/**< Find the width of a pixel row in bytes. */
} bmp_bitmap_callback_vt;

typedef struct bmp_image {
	bmp_bitmap_callback_vt bitmap_callbacks;	/**< callbacks for bitmap functions */
	unsigned int width;				/** width of BMP (valid after _analyse) */
	unsigned int height;				/** heigth of BMP (valid after _analyse) */
	bool decoded;					/** whether the image has been decoded */
	void *bitmap;					/** decoded image */
	/**	Internal members are listed below
	*/
	unsigned char *bmp_data;			/** pointer to BMP data */
	unsigned int buffer_size;			/** total number of bytes of BMP data available */
	bmp_encoding encoding;				/** pixel encoding type */
	unsigned int bitmap_offset;			/** offset of bitmap data */
	unsigned int bpp;				/** bits per pixel */
	unsigned int colours;				/** number of colours */
	unsigned int *colour_table;			/** colour table */
	bool reversed;					/** scanlines are top to bottom */
	bool ico;					/** image is part of an ICO, mask follows */
	unsigned int mask[4];				/** four bitwise mask */
	int shift[4];					/** four bitwise shifts */
} bmp_image;

struct ico_image {
	struct bmp_image bmp;
	struct ico_image *next;
};

struct ico_collection {
	unsigned int width;		/** width of largest BMP */
	unsigned int height;		/** heigth of largest BMP */
	/**	Internal members are listed below
	*/
	unsigned char *ico_data;	/** pointer to ICO data */
	unsigned int buffer_size;	/** total number of bytes of ICO data available */
  	struct ico_image *first;
};

void bmp_create(bmp_image *gif, bmp_bitmap_callback_vt *bitmap_callbacks);
bmp_result bmp_analyse(struct bmp_image *bmp);
bmp_result bmp_decode(struct bmp_image *bmp);
void bmp_finalise(struct bmp_image *bmp);

bmp_result ico_analyse(struct ico_collection *ico);
struct bmp_image *ico_find(struct ico_collection *ico, int width, int height);
void ico_finalise(struct ico_collection *ico);

#endif
