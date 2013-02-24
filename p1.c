#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "rdtsc.h"

#define MIN_READ_SZ	512
#define MAX_READ_SZ	(128*1024)	// Maximum read size, bytes
#define MAX_NUM_READS	1024
#define NUM_RUNS	1
#define STEP_TRANSFORM(x)	x += MIN_READ_SZ

// Vary read size 
unsigned int runExp(const char *fName, uint64_t *res){

	int fd;	// File descriptor 
	uint64_t t1, t2;

	off64_t offset;
	off64_t size;

	char buf[MAX_READ_SZ];
	unsigned int readSize;
	unsigned int index;

	if ( (fd = open(fName, O_RDONLY|O_LARGEFILE)) < 0){
		perror("fopen error");
		exit(-1);
	}

	// Get file size
	size = lseek(fd, 0, SEEK_END);

//	printf("Bytes Read, TSC Cycles\n");
	// Perform random read
	for (readSize = MIN_READ_SZ, index = 0; readSize < MAX_READ_SZ && index < MAX_NUM_READS && readSize < size; STEP_TRANSFORM(readSize), index++){
	//	system("sync; echo 3 >| /proc/sys/vm/drop_caches");
		
		// Read from random X * read Size
		offset = random() % (size - readSize);
		
//		offset = random() % (1024*1024);
		offset = offset - (offset % readSize);
//		printf("offset=%llu\n", offset);
		lseek(fd, offset, SEEK_SET);

		t1 = rdtsc_start();
		if(read(fd, buf, readSize) != readSize) break;	// Read i bytes
		t2 = rdtsc_end();

		if (res[index] == 0 || (t2-t1) < res[index]){
			res[index] = t2-t1;
		}
//		printf("%u,%llu\n", readSize, t2-t1);	// (bytes read, cycles)
	}
	
	close(fd);	

	return index;
}

int main(int argc, char *argv[]){
	uint64_t *intervals = calloc(MAX_NUM_READS, sizeof(uint64_t));

	unsigned int i, readSize;
	unsigned int maxInd;
	
	if (argc != 2){
		printf("Usage: p1 working_file\n");
		exit(-1);
	}

	// Run experiment
	for(i = 0; i < NUM_RUNS; i++){
		maxInd = runExp(argv[1], intervals);
	}

	// Print results
	printf("Read Buffer Size (B), TSC Cycles\n");
	for(i = 0, readSize=MIN_READ_SZ; i < maxInd; i++, STEP_TRANSFORM(readSize)){
		printf("%u, %llu\n", readSize, intervals[i] / readSize);
	}
	
	return 0;

}
