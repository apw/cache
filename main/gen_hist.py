import sys
import os
import matplotlib.pyplot as plt
import numpy as np
import pylab

PERCENTILE = 99.7
BINS = 100
SHAREX = True
SHAREY = True
SUBPLOT_DIST = 0.5
DATELEN = 19
PLOT_TYPE_INDEX = 1
FIRST_FILE_INDEX = 2

num_imps = len(sys.argv) - FIRST_FILE_INDEX

fig, axarr = plt.subplots(num_imps, sharex=SHAREX, sharey=SHAREY)

def create_hist(dat, h, m, col, lab, i, maximum):
    return axarr[i].hist(dat, bins=BINS, histtype='bar',
                         weights=np.ones_like(dat)/(float(len(h)) + float(len(m))),
                         color=[col],
                         label=[lab],
                         alpha=0.5,
                         range=(0,maximum))

# we plot either cpu counts or questions
plot_type = sys.argv[PLOT_TYPE_INDEX]
if (plot_type == "cpu_counts"):
    num_index = 1
elif (plot_type == "questions"):
    num_index = 2
else:
    assert(0)

for index in range(0, num_imps):
    # read in data from file
    fname = sys.argv[index+FIRST_FILE_INDEX]

    # make sure the two data files have the same date
    if (index == 0):
        date = fname[-DATELEN:]
    else:
        new_date = fname[-DATELEN:]
        assert(date == new_date)

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
        num = int(split_line[num_index])
        if (hm == "M"):
            m.append(num)
        elif (hm == "H"):
            h.append(num)
        else:
            assert(0)
                
#    h_max = np.percentile(h, PERCENTILE)
#    m_max = np.percentile(m, PERCENTILE)
    h_max = np.max(h)
    m_max = np.max(m)
    overall_max = max([h_max, m_max])

    new_h = [i for i in h if i <= h_max]
    new_m = [j for j in m if j <= m_max]
    n, bins, patches = create_hist(new_h, new_h, new_m,
                                   'blue', 'hits', index, overall_max)
    n, bins, patches = create_hist(new_m, new_h, new_m,
                                   'red', 'misses', index, overall_max)

    axarr[index].legend()
    axarr[index].set_title('Hit & Miss Distribution for \n'
                           + os.path.basename(fname))

pngname, fileext = os.path.splitext(os.path.basename(sys.argv[FIRST_FILE_INDEX]))
print pngname[-DATELEN:]
fig.subplots_adjust(hspace=SUBPLOT_DIST)
pylab.savefig('graphs/' + plot_type + pngname[-19:] + '.png')
