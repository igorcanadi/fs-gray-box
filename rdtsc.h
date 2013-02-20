#include <stdint.h>

inline uint64_t rdtsc_start(void) {
    unsigned cycles_high, cycles_low;

    __asm__ __volatile__("CPUID\n\t"
            "RDTSC\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t" : "=r" (cycles_high), "=r" (cycles_low):: "%rax", "%rbx", "%rcx", "%rdx");

    return (uint64_t)cycles_high << 32 | (uint64_t)cycles_low;
}

inline uint64_t rdtsc_end(void) {
    unsigned cycles_high, cycles_low;

    __asm__ __volatile__("RDTSCP\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t" : "=r" (cycles_high), "=r" (cycles_low):: "%rax", "%rbx", "%rcx", "%rdx");

    return (uint64_t)cycles_high << 32 | (uint64_t)cycles_low;
}
