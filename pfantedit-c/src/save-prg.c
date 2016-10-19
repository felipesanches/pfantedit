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
 *
 * (c) 2007, 2012 Felipe Corrêa da Silva Sanches <juca@members.fsf.org>
 *
 *------------------------ save-prg.c ------------------------------
 * The functions in this file implement encoding algorithms
 * for the .PRG file format. It includes, for instance, code to save
 * the background image of a pinball table or code to patch a 
 * .PRG file with an image you provide.
 *------------------------------------------------------------------
 */

#include <gtk/gtk.h>
#include <stdlib.h>
#include "prg.h"

//------------Prototypes-------------
void patch_prg(struct prg_contents *table);
void encode_masks(FILE *fp, struct prg_contents *table);
void write_palette(FILE *fp, struct prg_contents *table);
void encode_image(FILE *fp, unsigned char* image, unsigned int image_index, unsigned long int file_offset, int xsize, int ysize);
void encode_bgimage(FILE *fp, struct prg_contents *table);
extern char* new_filename_change_extension(char*,char*);
//-----------------------------------

void encode_masks(FILE *fp, struct prg_contents *table){
	unsigned int xplot,yplot,i,j;
	unsigned char byte;
	
        for(i=0;i<8;i++){
		xplot=1;
		yplot=1;
		fseek(fp,table->mask_addr[i],SEEK_SET); 
		printf("mask%d: %x-",i, (unsigned int) ftell(fp));		
        	while(yplot < 576){
			byte=0;
 			for(j=128;j>0;j/=2){
 				if (table->mask[i][320*(yplot-1) + xplot++ -1]) byte += j;
				if (xplot > 320){
					xplot=1;
					yplot++;
				}
			}
			putc(byte,fp);
		}
		printf("%x\n",(unsigned int) (ftell(fp)-1));
	}
}

void write_palette(FILE *fp, struct prg_contents *table){
	int i;
	fseek(fp, table->palette_addr, SEEK_SET);
	printf("Palette: %x-", (unsigned int) ftell(fp));
	for (i=0;i<256;i++){
		putc (table->palette[i].red, fp);		
		putc (table->palette[i].green, fp);
		putc (table->palette[i].blue, fp);
	}
	printf("%x\n", (unsigned int) (ftell(fp)-1));
}


static void dputc(unsigned char c,FILE* fp){
	printf("%x ",c);
}


static void compress(FILE *fp, int count, unsigned char *image, unsigned int *patch_index){
	unsigned int index=*(patch_index);
	while(count > 128){
		count-=128;
		putc(0x81,fp);
		putc(image[index],fp);
		index +=128;
	}
	putc(257-count,fp);
	putc(image[index],fp);
	
	index +=count;
	*(patch_index)=index;
}

static void raw(FILE *fp, int count, unsigned char *image, unsigned int *patch_index){
	int i, index=*(patch_index);
		
	while(count > 129){
		count-=129;
		putc(0x80,fp);
		i=129;
		while (i--){
			putc(image[index++],fp);
		}
	}
	
	putc(count-1,fp);
	
	while (count--){
		putc(image[index++],fp);
	}

	*(patch_index)=index;
}

static int consecutive_pixels(unsigned char* image, unsigned int *index, int xsize, int ysize, unsigned int base_index){
	int count=1;
	unsigned int i= *index;
	
	unsigned char prev_color;
	prev_color = image[i];
	while( image[++i] == prev_color){
		count++;
		if (i>= base_index + xsize*ysize){
			*(index)=i;
			return -count;
		}
	}
	*(index)=i;
	return count;
}

void encode_image(FILE *fp, unsigned char* image, unsigned int image_index, unsigned long int file_offset, int xsize, int ysize){

	int count, current, next;
	
	unsigned int patch_index=image_index, base_index=image_index;

	fseek(fp, file_offset, SEEK_SET);
	printf("%x-", (unsigned int) ftell(fp));

	current = consecutive_pixels(image, &image_index, xsize, ysize, base_index); //1st: 35 (F3)
	next = consecutive_pixels(image,  &image_index, xsize, ysize, base_index); //1st: 1 (03)

	count = 0;
//enum is_raw {FALSE, TRUE}  //???
	char is_raw=FALSE;
	if (current==1) is_raw=TRUE;//1st: FALSE

	while(current>0 && next>=0){
		if (current==1){
				if (is_raw==TRUE){
					count++;
				} else {
					compress(fp, count, image, &patch_index);
					count=1;
				}
				current = next;
				next = consecutive_pixels(image,  &image_index, xsize, ysize, base_index);
				is_raw=TRUE;
		} else {
			if (current>2) {
				if (count>0){
					if (is_raw==TRUE)
						raw(fp, count, image, &patch_index);
					else
						compress(fp, count, image, &patch_index);
				}
				count = current; //35
				current = next;//1
				next = consecutive_pixels(image,  &image_index, xsize, ysize, base_index); // 1st: 1 (08)
				is_raw=FALSE;
			} else { // current==2
				if (next>2){
					if (is_raw==TRUE){
						count+=2;
					} else {
						compress(fp, count, image, &patch_index);
						compress(fp, 2, image, &patch_index);
						count=0;
						is_raw=FALSE;
					}
					current=next;
					next = consecutive_pixels(image,  &image_index, xsize, ysize, base_index);
				} else {
					if (is_raw==TRUE){
						count+=current;
					} else {
						compress (fp, count, image, &patch_index);
						count=current;
					}
					current=next;
					next = consecutive_pixels(image,  &image_index, xsize, ysize, base_index);
					is_raw = TRUE;
				}
			}	
		}
	}

	next=-next;

	if (next==2){
		if (is_raw==TRUE){
			raw(fp, count+2, image, &patch_index);
		} else {
			compress(fp, count, image, &patch_index);
			compress(fp, 2, image, &patch_index);
		}
		return;
	}
	
	
	if (next>2){
		if (is_raw==TRUE)
			raw(fp, count, image, &patch_index);
		else
			compress(fp, count, image, &patch_index);
		compress(fp, next, image, &patch_index);
		return;
	}
	

	if (next==1){
		if (is_raw==TRUE){
			raw(fp, count+1, image, &patch_index);
		} else {
			compress(fp, count, image, &patch_index);
			raw(fp, 1, image, &patch_index);
		}
	}
	return;
}

void encode_bgimage(FILE *fp, struct prg_contents *table){
	int i;
	for (i=0;i<4;i++){
		printf("bgimage%d: ",i+1);
		encode_image(fp, table->bgimage, i*320*144, table->bgi_addr[i], 320, 144);
		printf("%x\n", (unsigned int) (ftell(fp)-1));
	}
}

void patch_prg(struct prg_contents *table){
	FILE *fp;
	//char *filename = new_filename_change_extension(table->filename,".NEW");
	char *filename = new_filename_change_extension(table->filename,".PRG"); //just a lazy hack...
	//printf("%s", filename);
	if ((fp = fopen(filename,"r+b"))<0){
		g_print("Error while trying to open %s\n", filename);
        exit(-1);
    }
	free(filename);

	encode_bgimage(fp, table);
	encode_masks(fp, table);
	write_palette(fp, table);
	fclose(fp);
}
