#include <stdio.h>
#include <stdlib.h>

#include "rdtsc.h"

#define MAX_READ_SZ	1024*1024

int main(int argc, char *argv[]){

	FILE *file;
	unsigned long long t1, t2;

	long int offset;
	long int size;

	char buf[MAX_READ_SZ];
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

	// Perform random read
	for (i = 256; i < MAX_READ_SZ && i < size; i+= 256){
		offset = random() % (size - i);
		fseek(file, offset, SEEK_SET);
		t1 = rdtsci_start();
		fread(buf, sizeof(char), i, file);
		t2 = rdtsc_end();

		printf("%u,%llu\n", i, t2-t1);
	}
	
	fclose(file);	

	return 0;

}
