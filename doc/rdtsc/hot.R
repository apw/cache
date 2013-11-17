dt <- read.table('hot.dat', header=F)

y <- t(dt[1, 1:100])

plot.ts(y,
        col='blue', 
        main='rdtsc calls with worker and cpuid warmup',
        xlab='Worker loop iterations', 
        ylab='cycles')
