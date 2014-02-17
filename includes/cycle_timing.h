/* Interface for counting CPU cycles in Intel x86 architecture.
 * Must call warmup_time before time_magic because Intel manual
 * specifies that cycle-counters need to warm up to a steady state
 * before being used. Otherwise, they will report very large numbers
 * at first, slowly reducing till it gets closer to the actual cycle times.
 */

#ifndef _CYCLE_TIMING_H
#define _CYCLE_TIMING_H

#include <stdint.h>

#include "../includes/rep.h"

// MUST CALL WARMUP_TIME BEFORE USING TIME_MAGIC
volatile int64_t warmup_time(void);

// returns the count of cycles for running a single query through a single
// cache representation
volatile int64_t time_magic(rep *r, unsigned (rep::*)(uint8_t *, unsigned), 
		   uint8_t *, unsigned, unsigned *);

#endif
