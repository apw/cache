import sys
import os
import matplotlib.pyplot as plt
import pylab

questionindex = 2

if (len(sys.argv) < 3):
    print 'Usage: python gen_scaling.py {num, len} dir'
    sys.exit(0)

dirname = sys.argv[2]

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


def capital(s):
    return s.title()

def get_imp(base_fname):
    imp_type = ""
    if "simple_cb" in base_fname:
        imp_type = "Contested Bytes"
    elif "in_order" in base_fname:
        imp_type = "In Order"
    elif "ll" in base_fname:
        imp_type = "Linked List"
    elif "trie_cb" in base_fname:
        imp_type = "Trie Contested Bytes"
    elif "carebear_dual_trie" in base_fname:
        imp_type = "Carebear Dual Trie"
    elif "carebear_forest" in base_fname:
        imp_type = "Carebear Forest"
    elif "lazy_exp" in base_fname:
        imp_type = "Lazy Exp Trie"
    elif "greedy_trie" in base_fname:
        imp_type = "Greedy Trie"
    elif "greedy_forest" in base_fname:
        imp_type = "Greedy Forest"
    elif "batch_forest" in base_fname:
        imp_type = "Batch Forest"
    else:
        print_err("No existing implementation type by this name", 5)
    
    return imp_type

i = 0
for imp in imps:
    for hm in ["hits", "misses"]:
        i = i + 1
        points = zip(imps[imp][hm]["x"], imps[imp][hm]["y"])
        maxX = max(imps[imp][hm]["x"])
        maxY = max(imps[imp][hm]["y"])
        sortedpoints = sorted(points)
        xs = [point[0] for point in sortedpoints]
        ys = [point[1] for point in sortedpoints]

        plt.figure(i)
        #plt.axhline(y=0)
        #plt.axvline(x=0)
        if (hm == "hits"):
            plt.plot(xs, ys, 'yo', markersize=12)
        if (hm == "misses"):
            plt.plot(xs, ys, 'ko', markersize=12)

        pylab.ylim([0, maxY*1.25])
        pylab.xlim([0, maxX*1.25])

        if (sys.argv[1] == 'num'):
            plt.xlabel('Number of Vectors in Cache')
            plt.title(get_imp(imp) + ' Scaling with Number of Cache Vectors (' + capital(hm) + ')')
        elif (sys.argv[1] == 'len'):
            plt.xlabel('Number of Bytenums in Cache & Query Vectors')
            plt.title(get_imp(imp) + ' Scaling with Number of Bytenums in Vectors (' + capital(hm) + ')')
        plt.ylabel('Total Number of Questions Asked')
        pylab.savefig('tmp/' + imp + '_' + hm + '.pdf')
        #plt.clf()

plt.show()


