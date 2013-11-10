#ifndef _CYCLE_TIMING_H
#define _CYCLE_TIMING_H

#include <stdint.h>

#include "../includes/rep.h"

volatile int64_t warmup_time(void);
volatile int64_t time_magic(rep *r, unsigned (rep::*)(uint8_t *, unsigned), 
		   uint8_t *, unsigned, unsigned *);

#endif
