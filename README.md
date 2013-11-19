# Cache research for CS 261

## 1 Introduction

## 2 Tutorial

### 2.1 Generate cache

Cache entries <em>c<sup>(i)</sup></em>
are elements of <em>{\*,0,1}<sup>+<sup></em>,
where <em>\*</em> is the "don't care" symbol that matches either *0* or *1*.
We generate a simple cache as:

```
$ echo 1 255 1 248 | rs 2 2 > c.dat
```

This produces a cache *c* of size *|c| = 2*,
whose two entries are of the form
<em>c<sup>(i)</sup> = (c<sub>0</sub><sup>(i)</sup>, ..., c<sub>n-1</sub><sup>(i)</sup>)</em>. In
this example, *n = 16*, and the two entries are
<em>c<sup>(0)</sup> = (\*,\*,\*,\*,\*,\*,\*,\*,1,1,1,1,1,1,1,1)</em> and
<em>c<sup>(1)</sup> = (\*,\*,\*,\*,\*,\*,\*,\*,0,0,0,1,1,1,1,1)</em>, since
we order the bits from least significant to most significant.

#### 2.1.1 Verify cache

The `-1` option causes `oracle` to load the cache into its internal
data structure, then dump that data structure and exit.  The following
command should produce no output:

```
$ diff -u -w <(cat c.dat) <(./oracle -1 -c c.dat)
```

#### 2.1.2 Verify cache raw code

The oracle assigns a *raw code* to each cache entry as it reads it.  Scanning
from left to right, when it encounters a bit that is *not* a don't care bit, it
extends the raw code for the current cache entry with the new bit index, using
the sign of the index to encode a specified value of *1* as *-1* and a
specified value of *0* as *+1* under the map
<em>c<sub>j</sub><sup>(i)</sup> = (-1)<sup>x<sub>j</sub><sup>(i)</sup></sup>.

The `-2` option causes `oracle` to load
the cache, assign the raw code, dump the raw code, then exit.  The reader can
verify the encoding of cache entries <em>c<sub>1</sub> and
<em>c<sub>2</sub> from above as follows:

```
$ ./oracle -2 -c c.dat
-8 -9 -10 -11 -12 -13 -14 -15 
+8 +9 +10 -11 -12 -13 -14 -15 
```

#### 2.1.3 Verify cache symbol code

The oracle assigns a *symbol code* to each cache entry by permuting each
raw code in such a way as to result in a valid binary search tree.

The `-3` option causes `oracle` to dump its symbol codes and exit:

```
$ ./oracle -3 -c c.dat
-15 -8 -13 -11 -14 -9 -10 -12 
-15 +8 +9 -12 -13 +10 -14 -11 
```

#### 2.1.4 Verify cache symbol code with dot plot

The `-4` option causes `oracle` to emit its symbol codes in the GraphViz
language appropriate for `dot`:

```
$ ./oracle -4 -c c.dat > c.dot
$ dot c.dot -T svg -o c.svg
```

The resultant plot for our example is:

![dot layout](doc/dot/c.png)

#### 2.1.5 Verify cache symbol code with force directed plot

When we have many cache entries of high dimension, the `dot` layout
engine is simply
not appropriate and we need to use the `fdp` layout engine instead.

The `-5` option causes `oracle` to emit its symbol codes in the GraphViz
language appropriate for `fdp`:

```
$ ./oracle -5 -c c.dat > c.dot
$ sfdp -Goverlap_scaling=-9 c.dot -T svg -o c.svg
```

The resultant plot for our example is:

<img src="doc/fdp/c.png" width="60%"/>

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
