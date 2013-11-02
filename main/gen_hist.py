import sys
import os
import matplotlib.pyplot as plt
import numpy as np
import pylab

PERCENTILE = 99.7
BINS = 100

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
    if (hm == "M"):
        m.append(num)
    elif (hm == "H"):
        h.append(num)
    else:
        assert(0)


h_max = np.percentile(h, PERCENTILE)
m_max = np.percentile(h, PERCENTILE)

def create_hist(dat, col, lab):
    return pylab.hist(dat, bins=BINS, histtype='bar',
                      weights=np.ones_like(dat)/float(len(dat)),
                      color=[col],
                      label=[lab],
                      alpha=0.5)

n, bins, patches = create_hist([i for i in h if i <= h_max],
                               'blue', 'hits')
n, bins, patches = create_hist([i for i in m if i <= m_max],
                               'red', 'misses')

pylab.legend()
pylab.title('Hit & Miss Distribution for \n' + os.path.basename(fname))

# store file in the graphs directory
pngname, fileext = os.path.splitext(os.path.basename(fname))
pylab.savefig('graphs/' + pngname + '.png')
#pylab.show()
