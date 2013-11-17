#include <stdio.h>
#include <stdint.h>

void hello(int I)
{
    int i;

    for (i = 0; i < I; i++) {
    }

}

int main(int argc, char *argv[])
{
    unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;
    int64_t start, end, dt[1000];
    int i;

    for (i = 0; i < 1000; i++) {

    asm volatile ("CPUID\n\t":::"%rax", "%rbx", "%rcx", "%rdx");
    asm volatile ("RDTSC\n\t"
                  "mov %%edx, %0\n\t"
                  "mov %%eax, %1\n\t"
                  :"=r" (cycles_high), "=r"(cycles_low)::"%rax", "%rdx");

    /*call the function to measure here */
    hello(i);

    asm volatile ("CPUID\n\t":::"%rax", "%rbx", "%rcx", "%rdx");
    asm volatile ("RDTSC\n\t"
                  "mov %%edx, %0\n\t"
                  "mov %%eax, %1\n\t"
                  :"=r" (cycles_high1), "=r" (cycles_low1)::"%rax", "%rdx");


    start = (((int64_t) cycles_high << 32) | cycles_low);

    end = (((int64_t) cycles_high1 << 32) | cycles_low1);

    dt[i] = end - start;
    }

    for (i = 0; i < 1000; i++) {
        printf("%lld ", dt[i]);
    }
    printf("\n");

    return 0;
}
