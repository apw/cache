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

#### 2.1.1 Verify cache

The `-1` option causes `oracle` to load the cache into its internal
data structure, then dump that data structure and exit.  The following
command should produce no output:

```
$ diff -u -w <(cat c.dat) <(./oracle -1 -c c.dat)
```

#### 2.1.1 Verify cache raw code

The oracle assigns a *raw code* to each cache entry as it reads it.  Scanning
from left to right, when it encounters a bit that is *not* a don't care bit, it
extends the raw code for the current cache entry with the new bit index, using
the sign of the index to encode a specified value of *1* as *-1* and a
specified value of *0* as *+1*.

The `-2` option causes `oracle` to load
the cache, assign the raw code, dump the raw code, then exit.  The reader can
verify the encoding of cache entries <em>c<sub>1</sub> and
<em>c<sub>2</sub> from above as follows:

```
$ ./oracle -2 -c c.dat
-8 -9 -10 -11 -12 -13 -14 -15 
-8 -9 -10 +11 +12 +13 +14 +15 
```

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
