# Cache research for CS 261

## Quick Start

    cd cache
    make

    wget http://people.seas.harvard.edu/~apw/cs261/cache/d02/q.dat.gz
    wget http://people.seas.harvard.edu/~apw/cs261/cache/d02/c.dat.gz

    gzcat c.dat.gz | head -128 > c.dat
    gzcat q.dat.gz | head -1024 > q.dat

    ./bench c.dat q.dat | tee t.dat

