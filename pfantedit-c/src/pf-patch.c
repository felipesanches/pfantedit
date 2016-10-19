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
 *----------------------------------------------------------------------
 */

#include <stdio.h>
#include "prg.h"

int main(int argc, char* argv[]){
	if (argc!=2){
		printf("usage: %s filename.prg\n",argv[0]);
		return -1;
	}
	
	struct prg_contents table;
	load_prg(argv[1],&table);	

	read_png(table.bgimage, "bgimage.png", table.palette);
	read_png(table.mask[0], "mask0.png", table.palette);
	read_png(table.mask[1], "mask1.png", table.palette);
	read_png(table.mask[2], "mask2.png", table.palette);
	read_png(table.mask[3], "mask3.png", table.palette);
	read_png(table.mask[4], "mask4.png", table.palette);
	read_png(table.mask[5], "mask5.png", table.palette);
	read_png(table.mask[6], "mask6.png", table.palette);
	read_png(table.mask[7], "mask7.png", table.palette);
	
	patch_prg(&table);
	return 0;
}
