dt <- read.table('cold.dat', header=F)

y <- t(dt[1, 1:100])

plot.ts(y,
        col='red', 
        main='rdtsc calls without warmup',
        xlab='Worker loop iterations', 
        ylab='cycles')
