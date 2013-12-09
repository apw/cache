import sys
import os
import matplotlib.pyplot as plt
import numpy as np
import pylab

PERCENTILE = 99.7
BINS = 40
SHAREX = True
SHAREY = True
SUBPLOT_DIST = 0.5
DATELEN = 19
PLOT_TYPE_INDEX = 1
FIRST_FILE_INDEX = 2
MAXHITX = 30000
MAXMISSX = 45000

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

for outer_index in range(0, 2): # do hits and misses separately
    for index in range(0, num_imps):
        # read in data from file
        fname = sys.argv[index+FIRST_FILE_INDEX]

        # make sure the two data files have the same date
        if (index == 0):
            date = fname[-DATELEN:]
        else:
            new_date = fname[-DATELEN:]
        #assert(date == new_date)

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
        if (outer_index == 0):
            #n, bins, patches = create_hist(new_h, new_h, new_m,
            #                               'yellow', 'hits', index, overall_max)
            n, bins, patches = create_hist(new_h, new_h, new_m,
                                           'yellow', 'hits', index, MAXHITX)
        else:
            #n, bins, patches = create_hist(new_m, new_h, new_m,
            #                               'black', 'misses', index, overall_max)
            n, bins, patches = create_hist(new_m, new_h, new_m,
                                           'black', 'misses', index, MAXMISSX)


        imp_type = ""
        h_or_m = ""
        base_fname = os.path.basename(fname)
        if "cb" in base_fname:
            imp_type = "Contested Byte"
        elif "in_order" in base_fname:
            imp_type = "In Order"
        elif "ll" in base_fname:
            imp_type = "Linked List"
        
        if (outer_index == 0):
            h_or_m = "Hit"
        else:
            h_or_m = "Miss"

        assert(imp_type != "")
        axarr[index].legend()
        axarr[index].set_title(h_or_m + ' Distribution for \n'
                               + imp_type)

    pngname, fileext = os.path.splitext(os.path.basename(sys.argv[FIRST_FILE_INDEX]))
    print pngname[-DATELEN:]
    fig.subplots_adjust(hspace=SUBPLOT_DIST)
    pylab.savefig('graphs/' + plot_type + pngname[-19:] + h_or_m + '.pdf', format='pdf')
    
    # reset axarr so get two different plots
    fig, axarr = plt.subplots(num_imps, sharex=SHAREX, sharey=SHAREY)
