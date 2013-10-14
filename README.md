# Cache research for CS 261

## Quick Start

    cd cache
    wget http://people.seas.harvard.edu/~apw/cs261/cache/d02/q.dat.gz
    gunzip q.dat.gz
    wget http://people.seas.harvard.edu/~apw/cs261/cache/d02/c.dat.gz
    gunzip c.dat.gz
    make
    ./bench c.dat q.dat | head -n 100 | tee t.dat

