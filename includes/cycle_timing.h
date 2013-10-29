#ifndef _CYCLE_TIMING_H
#define _CYCLE_TIMING_H

#include <stdint.h>

#include "../includes/rep.h"

int64_t warmup_time(void);
int64_t time_magic(rep *r, int (rep::*)(uint8_t *, unsigned), 
		   uint8_t *, unsigned, int *);

#endif
