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

#include <stdlib.h>
#include <stdio.h>
//     0          1             2             3                    4                5       
// ./rasura <este arquivo> <neste offset> <tantas vezes> <de tanto em tanto> <com este valor>
int main(int argc, char **argv){
	int count;
	unsigned long int offs;
	
	if (argc!=6){
		printf("forma de uso: %s <este arquivo> <neste offset> <tantas vezes> <de tanto em tanto> <com este valor>\n", argv[0]);
		return -1;
	}	

	FILE* fp = fopen(argv[1], "r+b");
	offs = atoi(argv[2]);
	fseek(fp,offs,SEEK_SET);
	count=atoi(argv[3]);
	while(count--){
		putc(atoi(argv[5]), fp);		
		fseek(fp,offs+=atoi(argv[4]),SEEK_SET);
	}
	return 0;
}

