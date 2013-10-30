import sys
import matplotlib.pyplot as plt
import numpy as np

# read in data from file
fname = sys.argv[1]
with open(fname) as f:
    lines = f.readlines()

# arrays for hits and misses
h = []
m = []

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
    
hist, bins = np.histogram(m, bins=20)
hist2, bins2 = np.histogram(h, bins=20)

N = 20
minm = min(m + h)
maxm = max(m + h)

h,b,c = plt.hist(h, N, range=(minm, maxm), log=True)
m,b,c = plt.hist(m, N, range=(minm, maxm), log=True)

plt.close('all')


ind = np.arange(N)  # the x locations for the groups
width = 0.35       # the width of the bars

fig, ax = plt.subplots()
rects1 = ax.bar(ind, h, width, color='b',log=True)

womenMeans = m
rects2 = ax.bar(ind+width, m, width, color='r',log=True)
rects = [rects2]
plt.show(rects)
