#include <stdio.h>
#include <stdlib.h>
#include "rdtsc.h"


#define BLOCK_SZ	256	// Bytes read each time

int main(int argc, char *argv[]){

	FILE *file;
	unsigned long long t1, t2;

	long int size, max_blocks;

	char buf[BLOCK_SZ];
	unsigned int i;

	if (argc != 2){
		perror("Provide file to read");
		exit(-1);
	}

	if ( (file = fopen(argv[1], "r")) == NULL){
		perror("fopen error");
		exit(-1);
	}

	// Get file file size
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	max_blocks = size / BLOCK_SZ;

	// Perform random read
	for (i = 0; i < max_blocks; i++){
		t1 = rdtsc();
		fread(buf, sizeof(char), BLOCK_SZ, file);
		t2 = rdtsc();

		printf("%u,%llu\n", i+1, t2-t1);	// (num blocks,cycles) 
	}
	
	fclose(file);	

	return 0;

}
