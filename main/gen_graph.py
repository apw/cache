import sys
import subprocess
import os

run_prepend = "../run/"
graph_prepend = "../graphs/"

gnuplot_file = run_prepend + "gen_graph.p"

title_index = 0;
date_index = 1;
misc_index = 2;
column_index = 3;

# read in data from file
fname = sys.argv[1]
with open(fname) as f:
    lines = f.readlines()

title = lines[title_index].replace("#", "").strip()
date = lines[date_index].replace("#", "").strip()
misc = lines[misc_index].replace("#", "").strip()
columns = lines[column_index].replace("#", "").strip().split(',');
split_fname = fname.split('/');
out_fname = graph_prepend + split_fname[len(split_fname)-1].split('.')[0]

command_string = "gnuplot -e \"infile='" + \
                 fname + "'; outfile='" + out_fname + \
                 ".png'; col1='" + columns[0] + \
                 "'; col2='" + columns[1] + "'\" > load " + gnuplot_file

# os.system is deprecated; should use subprocess class instead
print command_string
os.system(command_string)
