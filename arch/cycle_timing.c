/*
 * BADASS CODE FROM AMOS
 */

#include <stdint.h>
#include <stddef.h>

#include "../includes/cycle_timing.h"
#include "../includes/rep.h"

#define NULL_ITERATIONS (1 << 21)

// NULL function for measuring performance counter overhead
volatile static int null_func() {
  return 0;
}

/* Code for Yaniv and Neil for counting cycles on Intel platforms.  */
volatile int64_t time_magic(rep *r, unsigned (rep::*magic)(uint8_t *, unsigned),
		   uint8_t *arg1, unsigned arg2, unsigned *ret) {
  int64_t tic, toc, dt[1];
  unsigned cycles[4];
  
  /* Serialize execution.  */
  __asm volatile ("cpuid\n\t":::"%rax", "%rbx", "%rcx", "%rdx");
  
  /* Read time stamp counter.  */
  __asm volatile ("rdtsc\n\t"
		"mov %%edx, %0\n\t"
		"mov %%eax, %1\n\t":
		"=r" (cycles[0]),"=r"(cycles[1])::"%rax", "%rdx");

  /* Call your function whose cycle count you want to measure.  */
  *ret = (r->*magic)(arg1, arg2);

  /* Serialize execution.  */
  __asm volatile ("cpuid\n\t":::"%rax", "%rbx", "%rcx", "%rdx");

  /* Read time stamp counter.  */
  __asm volatile ("rdtsc\n\t"
		"mov %%edx, %0\n\t"
		"mov %%eax, %1\n\t":
		"=r" (cycles[2]),"=r"(cycles[3])::"%rax", "%rdx");

  /* Calculate and return elapsed cycles.  */
  tic = ((int64_t) cycles[0] << 32) | cycles[1];
  toc = ((int64_t) cycles[2] << 32) | cycles[3];
  dt[0] = toc - tic;
  return dt[0];
}

volatile static int64_t time_null_h(void) {
  int64_t tic, toc, dt[1];
  unsigned cycles[4];
  
  /* Serialize execution.  */
  __asm volatile ("cpuid\n\t":::"%rax", "%rbx", "%rcx", "%rdx");
  
  /* Read time stamp counter.  */
  __asm volatile ("rdtsc\n\t"
		"mov %%edx, %0\n\t"
		"mov %%eax, %1\n\t":
		"=r" (cycles[0]),"=r"(cycles[1])::"%rax", "%rdx");

  /* Call your function whose cycle count you want to measure.  */
  // TODO how can we verify that the compiler does not optimize this out?
  null_func();

  /* Serialize execution.  */
  __asm volatile ("cpuid\n\t":::"%rax", "%rbx", "%rcx", "%rdx");

  /* Read time stamp counter.  */
  __asm volatile ("rdtsc\n\t"
		"mov %%edx, %0\n\t"
		"mov %%eax, %1\n\t":
		"=r" (cycles[2]),"=r"(cycles[3])::"%rax", "%rdx");

  /* Calculate and return elapsed cycles.  */
  tic = ((int64_t) cycles[0] << 32) | cycles[1];
  toc = ((int64_t) cycles[2] << 32) | cycles[3];
  dt[0] = toc - tic;
  return dt[0];
}

volatile int64_t warmup_time(void) {
  int64_t res;
  for (int i = 0; i < NULL_ITERATIONS; i++) {
    res = time_null_h();
  }

  return res;
}
