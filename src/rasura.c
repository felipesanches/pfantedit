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

