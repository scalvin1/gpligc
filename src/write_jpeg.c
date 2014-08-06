/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 ******************** JPEG COMPRESSION INTERFACE ***************************
 ***************************************************************************/


#include <math.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>

//static const char *rcsid_write_jpeg_c =
//    "$Id: write_jpeg.c 3 2014-07-31 09:59:20Z kruegerh $";

struct my_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */
	jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr *my_error_ptr;

/*
 * Here's the routine that will replace the standard libjpeg error_exit method:
 */

METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	/* Always display the message. */
	/* We could postpone this until after returning, if we chose. */
	(*cinfo->err->output_message) (cinfo);

	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}

void
write_JPEG_file(int image_width, int image_height, int quality,
		char *jpegFileName, JSAMPLE * image_buffer)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	FILE *outfile;		/* target file */
	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	int row_stride;		/* physical row width in image buffer */

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	/* Here we use the library-supplied code to send compressed data to a
	 * stdio stream.  You can also write your own code to do something else.
	 * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	 * requires it in order to write binary files.
	 */

	if ((outfile = fopen(jpegFileName, "wb")) == NULL) {
		fprintf(stderr, "can't open %s\n", jpegFileName);
		exit(1);
	}
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = image_width;	/* image width and height, in pixels */
	cinfo.image_height = image_height;
	cinfo.input_components = 3;	/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;	/* colorspace of input image */

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality,
			 TRUE /* limit to baseline-JPEG values */ );
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = image_width * 3;	/* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] =
		    &image_buffer[cinfo.next_scanline * row_stride];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);

	fclose(outfile);
	jpeg_destroy_compress(&cinfo);
}

void
write_JPEG_stdout(int image_width, int image_height, int quality,
		  JSAMPLE * image_buffer)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	/*FILE * outfile; *//* target file */
	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	int row_stride;		/* physical row width in image buffer */

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	/* Here we use the library-supplied code to send compressed data to a
	 * stdio stream.  You can also write your own code to do something else.
	 * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	 * requires it in order to write binary files.
	 */

	/*
	   if ((outfile = fopen(jpegFileName, "wb")) == NULL) {
	   fprintf(stderr, "can't open %s\n", jpegFileName);
	   exit(1);
	   }
	 */

	jpeg_stdio_dest(&cinfo, stdout);

	cinfo.image_width = image_width;	/* image width and height, in pixels */
	cinfo.image_height = image_height;
	cinfo.input_components = 3;	/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;	/* colorspace of input image */

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality,
			 TRUE /* limit to baseline-JPEG values */ );
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = image_width * 3;	/* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] =
		    &image_buffer[cinfo.next_scanline * row_stride];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);

	/* fclose(outfile); */
	jpeg_destroy_compress(&cinfo);
}

unsigned char *read_jpg(int *width, int *height, char *filename)
{
	struct my_error_mgr jerr;
	struct jpeg_decompress_struct cinfo;
	/* struct my_error_mgr jerr; */
	FILE *infile;		/* source file */
	JSAMPARRAY buffer;	/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */
	int nr = 0;
	int tmpint = 0;
	unsigned char *map;

	if ((infile = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "Map::readMap_jpeg() can't open %s\n",
			filename);
		return NULL;
	}

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return NULL;
	}
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	(void) jpeg_read_header(&cinfo, TRUE);
	(void) jpeg_start_decompress(&cinfo);

	/* JSAMPLEs per row in output buffer */
	row_stride = cinfo.output_width * cinfo.output_components;

	map = malloc(3 * cinfo.output_width * cinfo.output_height);
	/*printf("width from jpg header = %d, height = %d, components = %d\n",
	   cinfo.output_width,cinfo.output_height,cinfo.output_components ); */
	*width = cinfo.output_width;
	*height = cinfo.output_height;
	/*
	   if(cinfo.output_components != 3){
	   printf("Map::readMap_jpeg() output_components = %d\n", cinfo.output_components);
	   }
	   if(cinfo.output_width != (unsigned int) width){
	   printf("Map::readMap_jpeg() output_width = %d\n", cinfo.output_width);
	   }
	   if(cinfo.output_height != (unsigned int) height){
	   printf("Map::readMap_jpeg() output_height = %d\n", cinfo.output_height);
	   }
	 */
	buffer = (*cinfo.mem->alloc_sarray)
	    ((j_common_ptr) & cinfo, JPOOL_IMAGE, row_stride, 1);
	while (cinfo.output_scanline < cinfo.output_height) {
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		/* printf("scanline %d\n",cinfo.output_scanline); */
		for (nr = 0; nr < (*width) * 3; nr++) {
			tmpint = (int) buffer[0][nr];
			/* printf("%d ",tmpint); */
			map[nr +
			    ((*width) * 3 * (cinfo.output_scanline - 1))] =
			    (unsigned char) tmpint;
			/* cout << map[nr] << endl; */
		}
		/* printf("\n"); */
	}
	(void) jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);
	return map;
}

void copy_resample_image_buffer
    (unsigned char *destination, unsigned char *source,
     int destination_x, int destination_y, int source_x, int source_y,
     int destination_width, int destination_height, int source_width,
     int source_height, int source_dimw, int destination_dimw) {
	/*
	   This function has been inspired and partially copied from an appropriate function in gd,
	   gdImageCopyResampled(), available in gd.c from http://www.boutell.com/gd/
	   2003 Jan Krueger

	   Usage: Only for true color (24bit) RGB arrays
	   We copy source buffer into destination buffer at the positions given in the
	   function call. The arrays must exist already.

	   I have tried to keep this function as universal as possible. Most parameters
	   will be 0 of width/height unless we are copying around image SECTIONS.
	 */

	int x, y;		/* mark the position at destination buffer */
	int source_array_pos, destination_array_pos;
	for (y = destination_y; (y < destination_y + destination_height);
	     y++) {
		/* over complete height of destination buffer */
		for (x = destination_x;
		     (x < destination_x + destination_width); x++) {
			/* over width of destination buffer scanline */
			float sy1, sy2, sx1, sx2;
			float sx, sy;
			float spixels = 0;
			float red = 0.0;
			float green = 0.0;
			float blue = 0.0;
			sy1 = ((float) y - (float) destination_y) * (float) source_height / (float) destination_height;	/* this is ycount * the y-scaling factor */
			sy2 =
			    ((float) (y + 1) -
			     (float) destination_y) *
			    (float) source_height /
			    (float) destination_height;
			sy = sy1;
			do {	/* while (sy < sy2)  [while we are between dest height pixels] */
				float yportion;
				if (floor(sy) == floor(sy1)) {
					yportion = 1.0 - (sy - floor(sy));
					if (yportion > sy2 - sy1) {
						yportion = sy2 - sy1;
					}
					sy = floor(sy);
				} else if (sy == floor(sy2)) {
					yportion = sy2 - floor(sy2);
				} else {
					yportion = 1.0;
				}
				sx1 =
				    ((float) x -
				     (float) destination_x) *
				    (float) source_width /
				    destination_width;
				sx2 =
				    ((float) (x + 1) -
				     (float) destination_x) *
				    (float) source_width /
				    destination_width;
				sx = sx1;
				do {	/* while (sx < sx2) [while we are between dest width pixels] */
					float xportion;
					float pcontribution;
					if (floor(sx) == floor(sx1)) {
						xportion =
						    1.0 - (sx - floor(sx));
						if (xportion > sx2 - sx1) {
							xportion =
							    sx2 - sx1;
						}
						sx = floor(sx);
					} else if (sx == floor(sx2)) {
						xportion =
						    sx2 - floor(sx2);
					} else {
						xportion = 1.0;
					}
					pcontribution =
					    xportion * yportion;
					source_array_pos =
					    3 * ((int) sx + source_x +
						 source_dimw * ((int) sy +
								source_y));
					red +=
					    source[source_array_pos] *
					    pcontribution;
					green +=
					    source[source_array_pos +
						   1] * pcontribution;
					blue +=
					    source[source_array_pos +
						   2] * pcontribution;
					spixels += xportion * yportion;
					sx += 1.0;
				} while (sx < sx2);
				sy += 1.0;
			} while (sy < sy2);
			if (spixels != 0.0) {
				red /= spixels;
				green /= spixels;
				blue /= spixels;
			}
			/* Avoid overflows by rounding errors above */
			if (red > 255.0) {
				red = 255.0;
			}
			if (green > 255.0) {
				green = 255.0;
			}
			if (blue > 255.0) {
				blue = 255.0;
			}
			destination_array_pos =
			    3 * (x + y * destination_dimw);
			destination[destination_array_pos] =
			    (unsigned char) red;
			destination[destination_array_pos + 1] =
			    (unsigned char) green;
			destination[destination_array_pos + 2] =
			    (unsigned char) blue;
		}
	}
}
