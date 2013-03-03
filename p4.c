#define _LARGEFILE64_SOURCE
#include "rdtsc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define MB 1024*1024LL
#define BLOCK_SIZE 4096
#define TO_SIZE 4000
#define RUNS 5
#define PUT_FIRST 1

uint64_t results[TO_SIZE + 1];

int main() {
    int err;
    uint64_t i, r;
    uint64_t tmp, cur_time;
    char *buffer;
    int fd;
    FILE *f;
    char test_file[] = "/scratch/dumpxx";

    buffer = (char *)malloc(BLOCK_SIZE);
    for (i = 0; i < BLOCK_SIZE; ++i) {
        buffer[i] = rand() % 128;
    }

    for (r = 0; r < RUNS; ++r) {
        test_file[13] = (r/10) % 10 + '0';
        test_file[14] = r % 10 + '0';
        unlink(test_file);
        for (i = 1; i <= TO_SIZE; ++i) {
            fd = open(test_file, O_WRONLY | O_SYNC | O_CREAT);
            fsync(fd);
            if (fd == -1) {
                fprintf(stderr, "syserror1\n");
                return 1;
            }
            err = lseek(fd, 0, SEEK_END);
            if (err == -1) {
                fprintf(stderr, "syserror2\n");
                return 1;
            }
            tmp = rdtsc_start();
            err = write(fd, buffer, PUT_FIRST);
            cur_time = rdtsc_end() - tmp;
            if (err != PUT_FIRST) {
                fprintf(stderr, "syserror3\n");
                return 1;
            }
            if (BLOCK_SIZE-PUT_FIRST) {
                err = write(fd, buffer+1, BLOCK_SIZE-PUT_FIRST);
                if (err != BLOCK_SIZE-PUT_FIRST) {
                    fprintf(stderr, "syserror3\n");
                    return 1;
                }
            }
            close(fd);

            if (r == 0 || results[i] > cur_time) {
                results[i] = cur_time;
            }
        }
    }

    for (i = 1; i <= TO_SIZE; ++i) {
        printf("%llu %llu\n", i*BLOCK_SIZE, results[i]);
    }

    return 0;
}
