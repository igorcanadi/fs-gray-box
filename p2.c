#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <fcntl.h>
#include "rdtsc.h"


#define READ_SZ		256	// Bytes read each time


int main(int argc, char *argv[]){

	int fd;			// File descriptor
	int numRead;		// Error status thing.	
	uint64_t t1, t2;	// Timers

	char buf[READ_SZ];	// Buffer
	unsigned int i;

	// Calling syntax: p2 clear_file working_file

	if (argc != 2){
		printf("Usage: p2 working_file\n");
		exit(-1);
	}

	if ( (fd = open(argv[1], O_RDONLY)|O_LARGEFILE) < 0){
		perror("open error");
		exit(-1);
	}

	// Cold start cache
	system("sync; echo 3 > /proc/sys/vm/drop_caches");
		
	numRead = READ_SZ;
	printf("# Blocks Read, TSC Cycles\n");
	// Perform successive sequential read
	for (i = 0; numRead == READ_SZ; i++){
		t1 = rdtsc_start();
		numRead = read(fd, buf, READ_SZ);
		t2 = rdtsc_end();

		printf("%u,%llu\n", i+1, t2-t1);	// (num blocks,cycles) 
	}
	
	close(fd);	

	return 0;

}
