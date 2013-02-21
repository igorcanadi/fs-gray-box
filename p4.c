#include "rdtsc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#define TEST_FILE "/scratch/.tmpfiledump"
#define BLOCK_SIZE 512
#define FROM_SIZE 1
#define TO_SIZE 200
#define RUNS 1

void put_junk(char *buffer) {
    int i;
    for (i = 0; i < BLOCK_SIZE; ++i) {
        buffer[i] = rand() % 128;
    }
}

int main() {
    int r;
    uint64_t i;
    char buffer[BLOCK_SIZE];
    uint64_t tmp, cur_time;
    uint64_t results[TO_SIZE+1] = {0};
    int fd;
   
    for (r = 0; r < RUNS; ++r) {
        unlink(TEST_FILE);
       
        fd = open(TEST_FILE, O_WRONLY | O_CREAT);
        for (i = FROM_SIZE; i <= TO_SIZE; ++i) {
            put_junk(buffer);
            tmp = rdtsc_start();
            // measure only one byte
            write(fd, buffer, 1);
            fsync(fd);
            results[i] += (rdtsc_end() - tmp);
            // put in the rest
            write(fd, buffer+1, BLOCK_SIZE-1);
            fsync(fd);
        }
        close(fd);
    }

    for (i = FROM_SIZE; i <= TO_SIZE; ++i) {
        // TODO WARN take a look at the data before you average it!
        printf("%d %llu\n", i, results[i] / RUNS);
    }

    return 0;
}
