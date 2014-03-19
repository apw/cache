import argparse
import sys
import os
import matplotlib.pyplot as plt
import numpy as np
import pylab

# TODO: units (1e7) don't transfer from bottom to to other axis
# TODO: for single graph, x-axis doesn't start at 0
# TODO: needs more thorough testing

PERCENTILE = 99.7
BINS = 100
SHAREX = True
SUBPLOT_DIST = 0.5

IN_HIST_X = 10
IN_PER_HIST_Y = 3

OUTPUT_DIR = '../output/'
GRAPHS_DIR = '../graphs/'

def print_parser_err(parser, msg, err):
    print "Error: " + msg + "."
    parser.print_help()
    sys.exit(err)

def print_err(msg, err):
    print "Error: " + msg + "."
    sys.exit(err)

def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument('--date', required=True)
    parser.add_argument('--type', default="questions")
    parser.add_argument('--sharey', action="store_true", default=False)
    parser.add_argument('--all', action="store_true", default=False)
    parser.add_argument('--except', nargs='+', type=str)
    parser.add_argument('--only', nargs='+', type=str)
    
    args = vars(parser.parse_args())
    
    # ensure either "all", "except" or "only" were specified
    tmp = 1 if args['all'] == False else 0
    tmp += 1 if args['except'] == None else 0
    tmp += 1 if args['only'] == None else 0
    if tmp != 2:
        print_parser_err(parser, "too many arguments", 2)

    global PLOT_TYPE
    PLOT_TYPE = args['type']
    global DATE
    DATE = args['date']
    global SHAREY
    SHAREY = args['sharey']
    
    # get all files from output directory
    raw_files = os.listdir(OUTPUT_DIR)
    
    # keep only files whose date matches this one
    all_output_files = [x for x in raw_files if DATE in x]
    
    if args['all'] == True:
        return all_output_files
    
    elif args['except'] != None:
        output_files = []
        for f in all_output_files:
            found = False
            for d in args['except']:
                if d in f:
                    found = True
                    break
            
            if not found:
                output_files.append(f)
        
        return output_files
    
    elif args['only'] != None:
        output_files = []
        for d in args['only']:
            for f in all_output_files:
                if d in f:
                    output_files.append(f)
        
        return output_files
    else:
        print_parser_err(parser, "weird error I don't understand", 3)

def get_imp(base_fname):
    imp_type = ""
    if "simple_cb" in base_fname:
        imp_type = "Contested Byte"
    elif "in_order" in base_fname:
        imp_type = "In Order"
    elif "ll" in base_fname:
        imp_type = "Linked List"
    elif "trie_cb" in base_fname:
        imp_type = "Trie Contested Byte"
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
    return imp_type


def create_hist(ax, data, w, col, lab, maximum):
    return ax.hist(data, bins=BINS, histtype='bar',
                    weights=w,
                    color=[col],
                    label=[lab],
                    alpha=0.5,
                    range=(0, maximum))

def create_fig(output_files, h_or_m):
    # function returns a single object if len(output_files) is 1 instead of list aghh
    if len(output_files) == 1:
        fig, tmp = plt.subplots(len(output_files), sharex=SHAREX, sharey=SHAREY)
        axarr = [tmp]
    else:
        fig, axarr = plt.subplots(len(output_files), sharex=SHAREX, sharey=SHAREY)

        
    if PLOT_TYPE == "questions":
        line_index = 1
    elif PLOT_TYPE == "cpu":
        line_index = 2
    else:
        assert(0)
    
    # collect all data from all files
    all_data = []
    for raw in output_files:
        data = []
        with open(OUTPUT_DIR + raw) as f:
            for line in f:
                if "#" in line:
                    continue
                
                split_line = line.split(" ")
                first = split_line[0]
                num = int(split_line[line_index])
                if (first == h_or_m):
                    data.append(num)
        
        all_data.append(data)
        
    assert(len(all_data) == len(output_files))
    
    # finds PERCENTILE in sum(all_data, []), which is all of the data concatenated
    maximum = np.percentile(sum(all_data, []), PERCENTILE)
    
    if h_or_m == "H":
        col = "yellow"
        lab = "hits"
        hit_or_miss = "Hit"
    else:
        col = "black"
        lab = "misses"
        hit_or_miss = "Miss"
    
    if PLOT_TYPE == "questions":
        xlab = "Number of Questions per Query"
    elif PLOT_TYPE == "cpu":
        xlab = "Number of CPU Cycles per Query"
    else:
        assert(0)
    
    for i in xrange(0, len(output_files)):
        w = np.ones_like(all_data[i])/(float(len(all_data[i]))),
        create_hist(axarr[i], all_data[i], w, col, lab, maximum)
        
        axarr[i].set_title(hit_or_miss + ' Distribution for ' + get_imp(output_files[i]))
        axarr[i].set_xlabel(xlab)
        axarr[i].set_ylabel('Prop of Queries')
        plt.setp(axarr[i].get_xticklabels(), visible=True)
    
    #fig.set_size_inches(10.5, 18.5)
    fig.set_size_inches(IN_HIST_X, IN_PER_HIST_Y * len(output_files) )
    fig.subplots_adjust(hspace=SUBPLOT_DIST)
    pylab.savefig(GRAPHS_DIR + DATE + "_" + PLOT_TYPE + "_" + hit_or_miss + '.pdf',
                  format='pdf')
    
def main():
    output_files = parse_arguments()
    
    if len(output_files) == 0:
        print_err("No output files selected", 4)
    
    create_fig(output_files, "H")
    create_fig(output_files, "M")
    
if __name__ == '__main__':
    main()
    

