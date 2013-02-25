#include <stdio.h>
#include <sys/time.h>
#include "rdtsc.h"

int main() {
    struct timeval tv, tv2;
    uint64_t startR, endR;
    uint64_t microsec = 0;
    uint64_t i, a = 0;

    gettimeofday(&tv, NULL);
    startR = rdtsc_start();
    for (i = 0; i < 800000000; ++i) {
        a += i;
    }
    endR = rdtsc_end();
    gettimeofday(&tv2, NULL);

    microsec = (tv2.tv_sec - tv.tv_sec)*1000000 + (tv2.tv_usec - tv.tv_usec);
    printf("%llu\n", microsec);
    printf("%llu %llu -> %llu\n", startR, endR, endR - startR);
    printf("freq: %lf\n", (double)(endR - startR) / microsec);
    printf("%llu\n", a);

    return 0;
}
