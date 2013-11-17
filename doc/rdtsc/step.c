#include <math.h>
#include <stdio.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;
    int64_t start, end, dt[1000];
    double r;
    int i;

    int n = 0;
    double d;
    int x;
    double mu = 0;
    double M = 0;
    double sigma = 1;

    do {
        asm volatile ("CPUID\n\t":::"%rax", "%rbx", "%rcx", "%rdx");
        asm volatile ("RDTSC\n\t"
                      "mov %%edx, %0\n\t"
                      "mov %%eax, %1\n\t":"=r" (cycles_high),
                      "=r"(cycles_low)::"%rax", "%rdx");

        /* do nothing  */

        asm volatile ("CPUID\n\t":::"%rax", "%rbx", "%rcx", "%rdx");
        asm volatile ("RDTSC\n\t"
                      "mov %%edx, %0\n\t"
                      "mov %%eax, %1\n\t":"=r" (cycles_high1),
                      "=r"(cycles_low1)::"%rax", "%rdx");

        start = (((int64_t) cycles_high << 32) | cycles_low);
        end = (((int64_t) cycles_high1 << 32) | cycles_low1);

        x = end - start;

        if (x > 500)
            continue;

        n = n + 1;
        d = x - mu;
        mu = mu + d / n;
        M = M + d * (x - mu);

        if (n > 1)
            sigma = sqrt(M / (n - 1));

        printf("%-6d %5d %12f %12f %12f %12f\n", n, x, d, mu, M, sigma);

    } while (n < 10000);

    return 0;
}
