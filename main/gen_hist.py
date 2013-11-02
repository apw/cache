import sys
import os
import matplotlib.pyplot as plt
import numpy as np
import pylab

# read in data from file
fname = sys.argv[1]
with open(fname) as f:
    lines = f.readlines()

# arrays for hits and misses
h = []
m = []

# read in each line, separating hits and misses
for line in lines:
    split_line = line.split(" ")
    hm = split_line[0]
    num = int(split_line[1])
    if (hm == "M"):
        m.append(num)
    elif (hm == "H"):
        h.append(num)
    else:
        assert(0)

# append the two lists into a meta-list
final_list = []
final_list.append(m)
final_list.append(h)

# plot the meta-list grouped by hit/miss
n, bins, patches = pylab.hist(final_list, 10, normed=1, histtype='bar',
                              color=['red', 'blue'],
                              label=['miss', 'hit'])

pylab.legend()
pylab.show()
