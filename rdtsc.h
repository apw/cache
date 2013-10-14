#include <limits.h>
#include <stdint.h>

/* Return the minimum observed overhead of the cycle count logic.  */
static inline int64_t rdtsc(int64_t lo)
{
    int const I = 1 << 18;
    int64_t tic, toc, dt;
    unsigned cycles[4];
    int i;

    /* If already calibrated just return the current cycle count.  */
    if (lo == 0) {
        /* Serialize execution.  */
        asm volatile ("cpuid\n\t":::"%rax", "%rbx", "%rcx", "%rdx");

        /* Read time stamp counter.  */
        asm volatile ("rdtsc\n\t"
                      "mov %%edx, %0\n\t"
                      "mov %%eax, %1\n\t":
                      "=r" (cycles[0]),"=r"(cycles[1])::"%rax", "%rdx");

        /* Combine the two registers into a single variable.  */
        tic = ((int64_t) cycles[0] << 32) | cycles[1];

        /* Return with the current cycle count.  */
        return tic;
    }

    /* Otherwise, enter the calibration loop.  */
    for (i = 0, lo = LONG_MAX; i < I; i++) {
        /* Serialize execution.  */
        asm volatile ("cpuid\n\t":::"%rax", "%rbx", "%rcx", "%rdx");

        /* Read time stamp counter.  */
        asm volatile ("rdtsc\n\t"
                      "mov %%edx, %0\n\t"
                      "mov %%eax, %1\n\t":
                      "=r" (cycles[0]),"=r"(cycles[1])::"%rax", "%rdx");

        /* Do nothing.  */

        /* Serialize execution.  */
        asm volatile ("cpuid\n\t":::"%rax", "%rbx", "%rcx", "%rdx");

        /* Read time stamp counter.  */
        asm volatile ("rdtsc\n\t"
                      "mov %%edx, %0\n\t"
                      "mov %%eax, %1\n\t":
                      "=r" (cycles[2]),"=r"(cycles[3])::"%rax", "%rdx");

        /* Calculate elapsed cycles.  */
        tic = ((int64_t) cycles[0] << 32) | cycles[1];
        toc = ((int64_t) cycles[2] << 32) | cycles[3];
        dt = toc - tic;

        /* Update minimum overhead.  */
        if (dt < lo)
            lo = dt;
    }

    /* Return minimum observed overhead.  */
    return lo;
}
