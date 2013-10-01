nothreads <- data.frame( read.table("nothreads.dat" ) )
names( nothreads ) <- c("time", "memory")

threads11 <-
  data.frame( cbind( 1:10, read.table("threads11.dat") ) )
names( threads11 ) <- c("n","time","memory")

threads10 <-
  data.frame( cbind( 1:10, read.table("threads10.dat") ) )
names( threads10 ) <- c("n","time","memory")

threads01 <-
  data.frame( cbind( 1:10, read.table("threads01.dat") ) )
names( threads01 ) <- c("n","time","memory")

pdf( "time.pdf", width=5, height=5 )
ymax <- max( c(threads10$time, threads01$time, threads11$time,
               nothreads$time ) )
plot(threads10$n, threads10$time, type="b", pch=19, col="blue",
     xlab="Threads", ylab="Time (s)", ylim=c(0,ymax) )
lines(threads01$n, threads01$time, col="black", type="b", pch=19 )
lines(threads11$n, threads11$time, col="red", type="b", pch=19 )
abline( h=nothreads$time, lty=2, col="gray" )
legend( "bottomright",
       legend=c("weights", "nodes", "weights+nodes",
         "no threads"), lty=c(1,1,1,2), pch=c(19,19,19,NA),
       col=c("black", "blue", "red", "gray"), bty="n" )
dev.off()

pdf( "memory.pdf", width=5, height=5 )
ymax <- max( c(threads10$memory, threads01$memory, threads11$memory,
               nothreads$memory ) )
ymin <- min( c(threads10$memory, threads01$memory, threads11$memory,
               nothreads$memory ) )
plot(threads10$n, threads10$memory / ymin, type="b", pch=19, col="blue",
     xlab="Threads", ylab="Memory Ratio", ylim=c(1,ymax/ymin) )
lines(threads01$n, threads01$memory / ymin, col="black", type="b",
      pch=19 )
lines(threads11$n, threads11$memory / ymin, col="red", type="b", pch=19 )
abline( h=nothreads$memory / ymin, lty=2, col="gray" )
legend( "bottomright",
       legend=c("weights", "nodes", "weights+nodes",
         "no threads"), lty=c(1,1,1,2), pch=c(19,19,19,NA),
       col=c("black", "blue", "red", "gray"), bty="n" )
dev.off()
