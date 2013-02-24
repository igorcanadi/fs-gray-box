#define _LARGEFILE64_SOURCE
#include "rdtsc.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define TEST_FILE "/scratch/.tmphugefile40GB"
#define GB 1024*1024*1204LL
#define FROM_SIZE GB
#define TO_SIZE 22*GB
#define STEP GB
#define RUNS 3
#define BUFFER_SIZE 20*1024*1024 // 20MB
#define BALLOON_SIZE 10*GB
#define DO_THE_BALLOONING

uint64_t get_avg_fetch_time(uint64_t size) {
    int r;
    uint64_t i;
    char *buffer;
    uint64_t tmp, total_time = 0, count_access = 0;
    int fd = open(TEST_FILE, O_RDONLY | O_LARGEFILE);
    if (fd == -1) {
        fprintf(stderr, "syserror\n");
        exit(1);
    }

    buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));

#ifdef DO_THE_BALLOONING
    /* initalize huge array */
    char *balloon = (char *)malloc(BALLOON_SIZE);
    for (i = 0; i < 1000000; ++i) {
        balloon[rand() % BALLOON_SIZE] = rand() % 128;
    }
    if (balloon == NULL) {
        fprintf(stderr, "syserror\n");
        exit(1);
    }
#endif

    /* Cold start cache */
    system("sync; echo 3 >| /proc/sys/vm/drop_caches");

    /* fill up the cache, no accounting */
    for (i = 0; i < size; ) {
        i += read(fd, buffer, BUFFER_SIZE);
    }
    close(fd);

    /* read the whole file few times, check if in cache or no */
    for (r = 0; r < RUNS; ++r) {
        fd = open(TEST_FILE, O_RDONLY | O_LARGEFILE);
        if (fd == -1) {
            fprintf(stderr, "syserror\n");
            exit(1);
        }
        for (i = 0; i < size; ) {
            tmp = rdtsc_start();
            i += read(fd, buffer, BUFFER_SIZE);
            total_time += (rdtsc_end() - tmp);
            ++count_access;
        }
        close(fd);
    }

#ifdef DO_THE_BALLOONING
        free(balloon);
#endif

    return total_time / count_access;
}

int main() {
    uint64_t size;

    for (size = FROM_SIZE; size <= TO_SIZE; size += STEP) {
        printf("%llu %llu\n", size, get_avg_fetch_time(size));
        fflush(stdout);
    }

    return 0;
}
