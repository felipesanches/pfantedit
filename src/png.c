/*----------------------------------------------------------------------
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *----------------------------------------------------------------------
 */

#include <gtk/gtk.h>
#include "prg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <unistd.h>

void write_png(char* raster, char *filename, struct color* pal){
  FILE *image;
  png_color palette[256];
  unsigned long width=320, height=576;
  int bitdepth=8;//, colourtype;
  png_uint_32 i;//, j, rowbytes=320;
  png_structp png;
  png_infop info;

	for (i=0;i<256;i++){
		palette[i].green=(png_byte)pal[i].green;
		palette[i].red=(png_byte)pal[i].red;
		palette[i].blue=(png_byte)pal[i].blue;
	}

  //png_bytepp row_pointers = NULL;
//char* row_pointers[height];
char** row_pointers;
  unsigned char sig[8];

row_pointers = (char **) malloc(height*sizeof(char *));
//row_pointers = (png_bytepp) malloc(width*rowbytes*sizeof(png_bytepp));

  // Get the image bitmap
  for (i = 0; i < height; ++i)
    //row_pointers[i] = raster + (i * rowbytes);
	row_pointers[i] = raster + (i * width);
  /////////////////////////////////////////////////////////////////////////////
  // Now write the image
  /////////////////////////////////////////////////////////////////////////////
  if((image = fopen(filename, "wb")) == NULL){
    fprintf(stderr, "Could not open the output image\n");
    return;
  }

  // Get ready for writing
  if ((png =
       png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL,
                                NULL)) == NULL){
    fprintf(stderr, "Could not create write structure for PNG (out of memory?)\n");
    return;
  }

  // Get ready to specify important stuff about the image
  if ((info = png_create_info_struct (png)) == NULL){
    fprintf(stderr,
	    "Could not create PNG info structure for writing (out of memory?)\n");
    return;
  }

  if (setjmp (png_jmpbuf (png))){
    fprintf(stderr, "Could not set the PNG jump value for writing\n");
    return;
  }

  // This is needed before IO will work (unless you define callbacks)
  png_init_io(png, image);

  // Define color palette
  png_set_PLTE(png,info,palette,256);

  // Define important stuff about the image
  png_set_IHDR (png, info, width, height, bitdepth, PNG_COLOR_TYPE_PALETTE,
                PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT);
  png_write_info (png, info);

  // Write the image out
  png_write_image (png, (png_byte **) row_pointers);

  // Cleanup
  png_write_end (png, info);
  png_destroy_write_struct (&png, &info);
  fclose(image);
}

void read_png(char* buf, char* filename, struct color* pal){
    unsigned char header[8];
    int width=320,height=576;
    int i,j;
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
	printf("ERROR opening .png file for reading.\n");
        return;
    }

    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
    {
	printf("Not a .png file!\n");
        return;
    }

    png_structp png_ptr = png_create_read_struct
       (PNG_LIBPNG_VER_STRING, NULL,NULL,NULL);
    if (!png_ptr){
	printf("Error - not enough memory (?)\n");
	return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr,
           (png_infopp)NULL, (png_infopp)NULL);
	printf("Error - not enough memory (?)\n");
	return;
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr,
          (png_infopp)NULL);
	printf("Error - not enough memory (?)\n");
	return;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr,
           &end_info);
        fclose(fp);
	printf("PNG Error.\n");
	return;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);


    png_read_info(png_ptr, info_ptr);


    char** row_pointers = (char **) malloc(height*sizeof(char *));

    for (i=0;i<height;i++){
	row_pointers[i]=buf + i*width;
    }

	


	png_read_image(png_ptr,(png_byte **) row_pointers);

	png_read_end(png_ptr, end_info);

	png_colorp png_palette;
	int num_palette = 0;
	png_get_PLTE(png_ptr, info_ptr, &png_palette, &num_palette);


	for (i=0;i<256;i++){
		pal[i].red=(unsigned char)png_palette[i].red;
		pal[i].green=(unsigned char)png_palette[i].green;
		pal[i].blue=(unsigned char)png_palette[i].blue;
	}

  fclose(fp);
}




