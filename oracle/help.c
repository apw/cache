#include <stdio.h>

/* Print usage statement.  */
void help(int argc, char *argv[])
{
    char format[] = "`%s' is a decision-theoretic framework\n\n"
        "Usage: %s [OPTIONS] -c CACHE -q QUERY\n\n"
        "Options:\n"
        " -h             Print this help statement then exit.\n"
        " -c CACHE       Read cache from file CACHE.\n"
        " -1             Print cache then exit.\n"
        " -2             Print raw code for each cache entry then exit.\n"
        " -3             Print symbol code for each cache entry then exit.\n"
        " -p DEPTH       Do not print DEPTH nodes at top of graph.\n"
        " -4             Print decision tree as a dot graph then exit.\n"
        " -5             Print decision tree as a fdp graph then exit.\n"
        " -q QUERY       Read queries from file QUERY.\n"
        " -6             Print queries then exit.\n"
        " -7             Print queries as boolean vectors then exit.\n"
        " -r             Use reference linked list instead of tree.\n\n"
        "Examples:\n"
        " echo 1 255 1 248 | rs 2 2 > c.dat\n"
        " echo 0 255 1 255 0 0 1 0 | rs 2 4 > q.dat\n"
        " %s -2 -c c.dat\n"
        " %s -3 -c c.dat\n"
        " %s -c c.dat -q q.dat\n";

    printf(format, argv[0], argv[0], argv[0], argv[0], argv[0]);
}
