# Cache research for CS 261

## 1 Introduction

## 2 Tutorial

### 2.1 Generate cache

Cache entries *c<sub>i<sub>* are elements of <em>{\*,0,1}<sup>+<sup></em>,
where <em>\*</em> is the "don't care" symbol that matches either *0* or *1*.
We generate a simple cache as:

```
$ echo 1 255 1 7 | rs 2 2 > c.dat
```

This produces a cache *c* of size *|c| = 2*,
whose two entries are
<em>c<sub>1</sub> = (\*,\*,\*,\*,\*,\*,\*,\*,1,1,1,1,1,1,1,1)</em> and
<em>c<sub>2</sub> = (\*,\*,\*,\*,\*,\*,\*,\*,1,1,1,0,0,0,0,0)</em>.

## 3 Telemetry

### 3.1 Intel cycle counting

In [arch/cycle_timing.c](arch/cycle_timing.c) is
our cycle counting logic.  Note that we
are careful to use execution barriers.

#### 3.1.1 Justification for calibration loop

We wrote [arch/cycle_timing.c](arch/cycle_timing.c) that way because the
Intel manual states that we has to "calibrate" `rdtsc` in two
ways.  First, we have to execute it enough times to settle into
a "steady state", and we have to estimate the minimum overhead imposed
by the two `cpuid` serializing instructions and the two `rdtsc`
instructions and the register copies, so we can subtract all
that off our measurements.  We imagine that you'd want to do
these two things in one calibration function that you call when
your program starts, where the calibration function returns
the minimum overhead.

See the following figure for what this looks like on one of our
server class machines for which the "steady state overhead"
is about 290 cycles.

![Instability](doc/rdtsc/step.pdf)

## References
