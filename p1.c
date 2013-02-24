#define _LARGEFILE64_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "rdtsc.h"

#define MIN_READ_SZ    512*1024
#define MAX_READ_SZ    (30*1024*1024) // Maximum read size, bytes
#define MAX_NUM_READS 1024*1024
#define NUM_RUNS    20
#define STEP_TRANSFORM(x)    x -= MIN_READ_SZ

// Vary read size 
unsigned int runExp(const char *fName, uint64_t *res){

    int fd;    // File descriptor 
    uint64_t t1, t2;
    int read_out;

    off64_t offset;
    off64_t size;

    char *buf = (char *)malloc(MAX_READ_SZ);
    unsigned int readSize;
    unsigned int index;

    // Perform random read
    for (readSize = MAX_READ_SZ, index = 0; readSize >= MIN_READ_SZ && index < MAX_NUM_READS; STEP_TRANSFORM(readSize), index++){
        system("sync; echo 3 >| /proc/sys/vm/drop_caches");
        if ( (fd = open(fName, O_RDONLY|O_LARGEFILE)) < 0){
            perror("fopen error");
            exit(-1);
        }
        size = lseek64(fd, -1024, SEEK_END) + 1024;
        if(read(fd, buf, 1) != 1){
            perror("read error");
            exit(-1);
        }
        
        // Read from random X * read Size
        offset = ((long long)random() * 100 + random()) % (size - readSize);
        
        offset = offset - (offset % readSize);
        lseek(fd, offset, SEEK_SET);

        t1 = rdtsc_start();
        read_out = read(fd, buf, readSize);
        t2 = rdtsc_end();
        if (read_out != readSize) {
            printf("# reads %d\n", index);
            break;    // Read i bytes
        }

        if (res[index] == 0 || (t2-t1) < res[index]){
            res[index] = t2-t1;
        }
        close(fd);    
    }

    free(buf);

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
    for(i = 0, readSize=MAX_READ_SZ; i < maxInd; i++, STEP_TRANSFORM(readSize)){
        printf("%u %lf\n", readSize, (double)intervals[i]/readSize);
    }
    
    return 0;

}
