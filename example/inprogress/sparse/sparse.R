dense <-
  data.frame( cbind( seq(.1,1,.1), read.table("dense.dat") ) )
names( dense ) <- c("nonzero","time","memory")

sparse <-
  data.frame( cbind( seq(.1,1,.1), read.table("sparse.dat") ) )
names( sparse ) <- c("nonzero","time","memory")

pdf( "time.pdf", width=5, height=5 )
plot(sparse$nonzero, sparse$time, type="b", pch=19,
     xlab="Fraction of non-zero elements",
     ylab="Time (s)",
     main="Feed-forward network, 1000x1000" )
lines(dense$nonzero, dense$time, type="b", pch=17, col=2)
legend( "bottomright", legend=c("Sparse", "Dense"),
       lty=c(1,1), pch=c(19,17), col=c(1,2) )
dev.off()

pdf( "memory.pdf", width=5, height=5 )
plot(sparse$nonzero, sparse$memory, type="b", pch=19,
     xlab="Fraction of non-zero elements",
     ylab="Memory (KB)",
     main="Feed-forward network, 1000x1000" )
lines(dense$nonzero, dense$memory, type="b", pch=17, col=2)
legend( "bottomright", legend=c("Sparse", "Dense"),
       lty=c(1,1), pch=c(19,17), col=c(1,2) )
dev.off()
