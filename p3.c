#include "rdtsc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#define TEST_FILE "/scratch/.tmphugefile40GB"
#define GB 1024*1024*1204LL
#define FROM_SIZE GB
#define TO_SIZE 20*GB
#define STEP GB
#define RUNS 3
#define BUFFER_SIZE 1024*1024 // 1MB
#define _GNU_SOURCE

uint64_t get_avg_fetch_time(uint64_t size) {
    int r;
    uint64_t i;
    char buffer[BUFFER_SIZE];
    uint64_t tmp, total_time = 0, count_access = 0;
    int fd = open(TEST_FILE, O_RDONLY | O_LARGEFILE);

    /* fill up the cache, no accounting */
    for (i = 0; i < size; ) {
        i += read(fd, buffer, BUFFER_SIZE);
    }
    close(fd);

    /* read the whole file few times, check if in cache or no */
    for (r = 0; r < RUNS; ++r) {
        fd = open(TEST_FILE, O_RDONLY | O_LARGEFILE);
        for (i = 0; i < size; ) {
            tmp = rdtsc_start();
            i += read(fd, buffer, BUFFER_SIZE);
            total_time += (rdtsc_end() - tmp);
            ++count_access;
        }
        close(fd);
    }

    return total_time / count_access;
}

int main() {
    uint64_t size;

    for (size = FROM_SIZE; size <= TO_SIZE; size += STEP) {
        printf("%llu %llu\n", size, get_avg_fetch_time(size));
    }

    return 0;
}
