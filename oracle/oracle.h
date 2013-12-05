#include "tunnel.h"
#include "vertex.h"
#include <gsl/gsl_randist.h>

enum {
    ERROR  = 1 << 1,
    HELP   = 1 << 2,
    CACHE  = 1 << 3,
    RAW    = 1 << 4,
    SYMBOL = 1 << 5,
    QUERY  = 1 << 6,
    DOT    = 1 << 7,
    FDP    = 1 << 8,
    LIST   = 1 << 9,
    BITS   = 1 << 10,
};

void help(int argc, char *argv[]);
int solve(tunnel *t, gsl_rng *r);
tunnel *walk(uint8_t const *x, vertex *root);
int sgn(int i);
int tree(vertex * root, tunnel * cache);
int bit(uint8_t const *x, int j);
void dot(tunnel * cache, int prune);
void fdp(tunnel * cache, int prune);
