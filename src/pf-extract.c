#include <stdio.h>
#include "prg.h"

int main(int argc, char* argv[]){
	if (argc!=2){
		printf("usage: %s filename.prg\n",argv[0]);
		return -1;
	}

	struct prg_contents table;

	load_prg(argv[1],&table);
	
	write_png(table.bgimage, "bgimage.png", table.palette);
	write_png(table.mask[0], "mask0.png", table.palette);
	write_png(table.mask[1], "mask1.png", table.palette);
	write_png(table.mask[2], "mask2.png", table.palette);
	write_png(table.mask[3], "mask3.png", table.palette);
	write_png(table.mask[4], "mask4.png", table.palette);
	write_png(table.mask[5], "mask5.png", table.palette);
	write_png(table.mask[6], "mask6.png", table.palette);
	write_png(table.mask[7], "mask7.png", table.palette);
	
	return 0;
}
