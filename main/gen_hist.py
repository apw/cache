import sys
import os
import matplotlib.pyplot as plt
import numpy as np
import pylab

PERCENTILE = 99.7
BINS = 100
SHAREX = False
SHAREY = True
SUBPLOT_DIST = 0.5
DATELEN = 19
FIRST_FILE_INDEX = 1

num_imps = len(sys.argv) - FIRST_FILE_INDEX

fig, axarr = plt.subplots(num_imps, sharex=SHAREX, sharey=SHAREY)

def create_hist(dat, col, lab, i):
    return axarr[i].hist(dat, bins=BINS, histtype='bar',
                         weights=np.ones_like(dat)/float(len(dat)),
                         color=[col],
                         label=[lab],
                         alpha=0.5)

for index in range(0, num_imps):
    print index
    # read in data from file
    fname = sys.argv[index+FIRST_FILE_INDEX]

    # make sure the two data files have the same date
    if (index == 0):
        dat = fname[-DATELEN:]
    else:
        new_dat = fname[-DATELEN:]
        assert(dat == new_dat)

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

        n, bins, patches = create_hist([i for i in h if i <= h_max],
                                       'blue', 'hits', index)
        n, bins, patches = create_hist([i for i in m if i <= m_max],
                                       'red', 'misses', index)


        axarr[index].legend()
        axarr[index].set_title('Hit & Miss Distribution for \n'
                               + os.path.basename(fname))

pngname, fileext = os.path.splitext(os.path.basename(sys.argv[FIRST_FILE_INDEX]))
print pngname[-DATELEN:]
fig.subplots_adjust(hspace=SUBPLOT_DIST)
pylab.savefig('graphs/' + pngname[-19:] + '.png')
