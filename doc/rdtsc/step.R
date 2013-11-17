y <- read.table('step.dat', header=F)

par(mfrow=c(1,2))

plot.ts(y[1:5000,]$V2, col='blue', xlab='Observations', ylab='Cycles',
             main=expression('Time series of Intel rdtsc'~Delta*t))

lines(y[1:5000,]$V4, col='red')

abline(mean(y$V2), 0, col='green')

legend(2500, 443, 
       lty=c(1,1,1),
       col=c('blue', 'red', 'green'), 
       c(expression(Delta*t~'observation'),
         expression(Delta*t~'sample mean'),
         expression(Delta*t~'population mean')))
         
hist(y[1:5000,]$V2, 
     100, 
     main=expression('Empirical'~Delta*t~'distribution'),
     xlab=expression(Delta*t),
     probability=T,
     col='blue')         