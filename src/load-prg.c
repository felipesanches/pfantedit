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

 *-------------------------- load-prg.c -------------------------------------
 * The functions in this file implement decoding algorithms for the .PRG 
 * file format. It includes, for exemple, code to extract the background image
 * of a pinball table.
 *----------------------------------------------------------------------
 */

#include <gtk/gtk.h>
#include "prg.h"
#define IMAGE_XMAX 320

//------------------Prototypes--------------------------
void load_prg(char* filename, struct prg_contents *table);
void load_palette(FILE *fp, struct prg_contents *table);
void decode_bgimage(FILE *fp, struct prg_contents *table);
void decode_masks(FILE *fp, struct prg_contents *table);
void decode_image(FILE *fp, unsigned long int file_offset, unsigned char* image, int xsize, int ysize);
void decode_mask(FILE *fp, char* mask, int xsize, int ysize, unsigned int addr);
extern void parsexml(char *docname, struct prg_contents* table);
extern char* new_filename_change_extension(char*,char*);
//------------------------------------------------------

void load_prg(char* filename, struct prg_contents *table){
	FILE *fp;
	int i,j;
	char *conf_file = new_filename_change_extension(filename,".xml");

	parsexml(conf_file, table);
	free(conf_file);

	if ((fp = fopen(filename,"r"))<0){
                g_print("Error while trying to open %s\n", filename);
                exit(-1);
        }

	//strcopy (table->filename, filename);
	i=0;
	while((table->filename[i] = filename[i]) != '\0')
		i++;

	decode_bgimage(fp, table);
	decode_masks(fp, table);
	load_palette(fp, table);
	fclose(fp);
}

void decode_image(FILE *fp, unsigned long int file_offset, unsigned char* image, int xsize, int ysize){
	int xplot=0, yplot=0, i;
	unsigned char count,color;

        fseek(fp,file_offset,SEEK_SET);
	printf("image: %x-",ftell(fp));
	
        while(yplot < ysize){
		count = getc(fp);
		if (count <= 0x80){
                        count++;
			while(count--){
				image[xsize*yplot + xplot++] = getc(fp);
				if (xplot >= xsize){
					xplot=0;
					yplot++;
				}
			}
		} else {
			color = getc(fp);
			for(i=0;i<257-count;i++){
				image[xsize*yplot + xplot++] = color;
				if (xplot >= xsize){
					xplot=0;
					yplot++;
				}
			}
		}
        }
	printf("%x\n",ftell(fp)-1);
}



void decode_bgimage(FILE *fp, struct prg_contents *table){
	int i;
	for(i=0;i<4;i++){
		decode_image(fp, table->bgi_addr[i], &(table->bgimage[i*320*144]), 320, 144);
	}
}




void decode_mask(FILE *fp, char* mask, int xsize, int ysize, unsigned int addr){	
	unsigned int xplot=1,yplot=1,j;
	unsigned char byte;
	
	fseek(fp,addr,SEEK_SET);
	printf("%x-",ftell(fp));
	while(yplot < ysize){
		byte = getc(fp);
		for(j=128;j>0;j/=2){
			mask[xsize*(yplot-1) + xplot++ -1] = (byte & j)/j;
			if (xplot > xsize){
				xplot=1;
				yplot++;
			}
		}
	}
	
}

void decode_masks(FILE *fp, struct prg_contents *table){	
	int i;
	
        for(i=0;i<8;i++){
		printf("mask%d: ",i);
		decode_mask(fp, table->mask[i], 320, 576, table->mask_addr[i]);
		printf("%x\n",ftell(fp)-1);
	}
	printf("flipper: ");
	decode_mask(fp, table->flipper, 64, 576, table->flipper_addr);
	printf("%x\n",ftell(fp)-1);
}

void load_palette(FILE *fp, struct prg_contents *table){
	int i;
	fseek(fp, table->palette_addr, SEEK_SET);
	printf("Palette: %x-", ftell(fp));
	for (i=0;i<256;i++){
		table->palette[i].red = getc(fp);		
		table->palette[i].green = getc(fp);
		table->palette[i].blue = getc(fp);
	}
	printf("%x\n", ftell(fp)-1);
}
