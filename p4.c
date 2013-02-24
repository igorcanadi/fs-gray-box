#define _LARGEFILE64_SOURCE
#include "rdtsc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define TEST_FILE "/scratch/.tmpfiledump"
#define BLOCK_SIZE 128
#define FROM_SIZE 1
#define TO_SIZE 10000
#define RUNS 5

void put_junk(char *buffer) {
    int i;
    for (i = 0; i < BLOCK_SIZE; ++i) {
        buffer[i] = rand() % 128;
    }
}

int main() {
    int r, t;
    uint64_t i;
    char buffer[BLOCK_SIZE];
    uint64_t tmp, cur_time;
    uint64_t results[TO_SIZE+1] = {0};
    int fd;
   
    for (r = 0; r < RUNS; ++r) {
        unlink(TEST_FILE);
        fd = open(TEST_FILE, O_WRONLY | O_SYNC | O_CREAT);
        if (fd == -1 || fsync(fd) == -1) {
            fprintf(stderr, "syserror\n");
            return 1;
        }
        for (i = FROM_SIZE; i <= TO_SIZE; ++i) {
            put_junk(buffer);
            tmp = rdtsc_start();
            // measure only one byte
            t = write(fd, buffer, 1);
            cur_time = rdtsc_end() - tmp;
            if (r == 0 || results[i] > cur_time) {
                results[i] = cur_time;
            }
            if (t != 1) {
                fprintf(stderr, "syserror\n");
                return 1;
            }

            // put in the rest
            t = write(fd, buffer+1, BLOCK_SIZE-1);
            if (t != BLOCK_SIZE-1) {
                fprintf(stderr, "syserror\n");
                return 1;
            }
        }
        close(fd);
    }

    for (i = FROM_SIZE; i <= TO_SIZE; ++i) {
        printf("%llu %llu\n", i, results[i]);
    }

    return 0;
}
