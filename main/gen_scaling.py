import sys
import os
import matplotlib.pyplot as plt
import pylab

questionindex = 2

if (len(sys.argv) < 2):
    print 'Usage: python gen_scaling.py dir'
    sys.exit(0)

dirname = sys.argv[1]

imps = {}

'''
expects files to be in numbered directories, where the number is the
length of the vectors. All such numbered directories should be in the
directory passed in; also expects files to be dated with _2 immediately
after the rep name
'''

numreps = 0

# iterate over numbered directories
for num in os.listdir(dirname):
    # for each rep in that directory
    for filename in os.listdir(dirname + num):
        repname = filename.split('_2')[0]
        if (repname not in imps):
            imps[repname] = {}
            imps[repname]["hits"] = {}
            imps[repname]["hits"]["x"] = []
            imps[repname]["hits"]["y"] = []
            imps[repname]["misses"] = {}
            imps[repname]["misses"]["x"] = []
            imps[repname]["misses"]["y"] = []

            numreps = numreps + 1

        hitsum = 0
        misssum = 0

        # get total number of questions 
        with open(dirname + num + '/' + filename) as fp:
            for line in fp:
                if (line[0] == 'H'):
                    hitsum = hitsum + int(line.split(' ')[questionindex])
                elif (line[0] == 'M'):
                    misssum = misssum + int(line.split(' ')[questionindex])

        # store the points we will want to plot (but only if sum > 0, meaning
        # there was at least one data point
        if (hitsum > 0):
            imps[repname]["hits"]["x"].append(int(num))
            imps[repname]["hits"]["y"].append(hitsum)
        if (misssum > 0):
            imps[repname]["misses"]["x"].append(int(num))
            imps[repname]["misses"]["y"].append(misssum)


i = 0
for imp in imps:
    for hm in ["hits", "misses"]:
        i = i + 1
        points = zip(imps[imp][hm]["x"], imps[imp][hm]["y"])
        sortedpoints = sorted(points)
        xs = [point[0] for point in sortedpoints]
        ys = [point[1] for point in sortedpoints]

        plt.figure(i)
        plt.plot(xs, ys)
        plt.xlabel('number of vectors in cache')
        plt.ylabel('total number of questions asked')
        plt.title(imp + ' scaling with number of vectors ' + hm)
        pylab.savefig('tmp/' + imp + '_' + hm + '.pdf')
        #plt.clf()

plt.show()


