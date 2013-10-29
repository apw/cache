set terminal png size 800,600 enhanced font "Helvetica,20"
set output outfile
plot  infile using 1:1 title col1, infile using 1:2 title col2