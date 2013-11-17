dt <- read.table('warm.dat', header=F)

y <- t(dt[1, 1:100])

plot.ts(y,
        col='orange', 
        main='rdtsc calls with worker warmup',
        xlab='Worker loop iterations', 
        ylab='cycles')
