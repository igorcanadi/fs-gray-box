#define _LARGEFILE64_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "rdtsc.h"


#define READ_SZ		512	// Bytes read each time
#define MAX_READS	4096	// Maximum number of reads

#define OFFSET_STEP	512

#define NUM_RUNS	100

#define EXP	1

void runExp(const char *fName, uint64_t *res){

	int fd;			// File descriptor
	uint64_t t1, t2;	// Timers

	char buf[READ_SZ];	// Buffer
	unsigned int i;


	if ( (fd = open(fName, O_RDONLY|O_LARGEFILE)) < 0){
		perror("open error");
		exit(-1);
	}

	// Cold start cache
	system("sync; echo 3 >| /proc/sys/vm/drop_caches");
		
//	printf("# Reads, TSC Cycles\n");
	// Perform successive sequential read
	for (i = 0; i < MAX_READS; i++){
		t1 = rdtsc_start();
		if (read(fd, buf, READ_SZ) <= 0) break;
		t2 = rdtsc_end();
		if ( res[i] == 0 || (t2-t1) < res[i]) {
			res[i] = t2-t1;
		}
//	printf("%u,%llu\n", i+1, t2-t1);	// (num blocks,cycles) 
	}
	
	close(fd);	

}

// 1 byte read appoach
// Read 1 byte from position X, Read 1 byte from position X + Y (time second read)
// plot time vs Y
void runExp2(const char *fName, uint64_t *res){

	int fd;			// File descriptor
	uint64_t t1, t2;	// Timers

	char buf[READ_SZ];	// Buffer
	unsigned int i;
	
	off64_t offset;

	if ( (fd = open(fName, O_RDONLY|O_LARGEFILE)) < 0){
		perror("open error");
		exit(-1);
	}

	// Cold start cache
	system("sync; echo 3 >| /proc/sys/vm/drop_caches");
	
//	printf("Read Offset, TSC Cycles\n");
	// Read 1 byte, set offset.
	for (i = 0, offset = 0; i < MAX_READS; i++, offset+=OFFSET_STEP){
	
		lseek(fd, 0, SEEK_SET);			// Rewind to beginning
		if (read(fd, buf, 1) <= 0) break;	// Read 1 B
		lseek(fd, offset, SEEK_SET);		// Skip to Y
		
		t1 = rdtsc_start();		
		if (read(fd, buf, 1) <= 0) break;
		t2 = rdtsc_end();
		
		if ( res[i] == 0 || (t2-t1) < res[i]) {
			res[i] = t2-t1;
		}
//		printf("%u,%llu\n", offset, t2-t1);	// (num blocks,cycles) 
	}
	
	close(fd);	

}
int main(int argc, char *argv[]){
	uint64_t *intervals = calloc(MAX_READS, sizeof(uint64_t));

	unsigned int i;
	
	// Calling syntax: p2 clear_file working_file

	if (argc != 2){
		printf("Usage: p2 working_file\n");
		exit(-1);
	}

	// Run experiment
	for(i = 0; i < NUM_RUNS; i++){
#if (EXP == 0)
		runExp(argv[1], intervals);
#else
		runExp2(argv[1], intervals);
#endif
	}

	// Print results
#if (EXP == 0)
	printf("# Reads of Size %u, TSC Cycles\n", READ_SZ);
#else
	printf("Offset (x %uB), TSC Cycles\n", OFFSET_STEP);
#endif
	for(i = 0; i < MAX_READS; i++){
		printf("%u, %llu\n", i+1, intervals[i]);
	}

	return 0;
}
