import sys
import os
import matplotlib.pyplot as plt
import numpy as np
import pylab

MAX_PERCENT_OUTLIERS = 0.005
OUTLIER_LATENCY = 1400000

# read in data from file
fname = sys.argv[1]
with open(fname) as f:
    lines = f.readlines()

# arrays for hits and misses
h = []
m = []

num_outliers = 0

# read in each line, separating hits and misses
for line in lines:
    split_line = line.split(" ")
    hm = split_line[0]
    num = int(split_line[1])
    if (num > OUTLIER_LATENCY):
        num_outliers = num_outliers + 1
        continue
    if (hm == "M"):
        m.append(num)
    elif (hm == "H"):
        h.append(num)
    else:
        assert(0)

print "Number of Outliers = " + str(num_outliers) + " of " + str(len(m) + len(h))
assert(num_outliers < (len(m) + len(h)) * MAX_PERCENT_OUTLIERS)

def create_hist(dat, col, lab):
    return pylab.hist(dat, bins=100, histtype='bar',
                      weights=np.ones_like(dat)/float(len(dat)),
                      color=[col],
                      label=[lab],
                      alpha=0.5)

n, bins, patches = create_hist(h, 'blue', 'hits')
n, bins, patches = create_hist(m, 'red', 'misses')

pylab.legend()
pylab.title('Hit & Miss Distribution for \n' + os.path.basename(fname))

# store file in the graphs directory
pngname, fileext = os.path.splitext(os.path.basename(fname))
pylab.savefig('graphs/' + pngname + '.png')
#pylab.show()
