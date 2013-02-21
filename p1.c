#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "rdtsc.h"

#define MAX_READ_SZ	(1024*1024)	// Maximum read size, bytes

int main(int argc, char *argv[]){

	int fd;	// File descriptor 
	uint64_t t1, t2;

	long int offset;
	long int size;

	char buf[MAX_READ_SZ];
	unsigned int i;

	if (argc != 2){
		printf("Usage: p1 working_file\n");
		exit(-1);
	}

	if ( (fd = open(argv[1], O_RDONLY|O_LARGEFILE)) < 0){
		perror("fopen error");
		exit(-1);
	}

	// Get file size
	size = lseek(fd, 0, SEEK_END);

	printf("Bytes Read, TSC Cycles\n");
	// Perform random read
	for (i = 256; i < MAX_READ_SZ && i < size; i <<= 1){
		offset = random() % (size - i);
		lseek(fd, offset, SEEK_SET);
		t1 = rdtsc_start();
		if(read(fd, buf, i) != i) break;	// Read i bytes
		t2 = rdtsc_end();

		printf("%u,%llu\n", i, t2-t1);	// (bytes read, cycles)
	}
	
	close(fd);	

	return 0;

}
