threads14 <-
  data.frame( cbind( seq(1,20,), read.table("threads-w=1-t=4.dat") ) )
names( threads14 ) <- c("n","time","memory")

threads24 <-
  data.frame( cbind( seq(1,20,), read.table("threads-w=2-t=4.dat") ) )
names( threads24 ) <- c("n","time","memory")

threads26 <-
  data.frame( cbind( seq(1,20,), read.table("threads-w=2-t=6.dat") ) )
names( threads26 ) <- c("n","time","memory")

pdf( "time-netlevel.pdf", width=5, height=5 )
plot(threads24$n, threads24$time, type="b", pch=19,
     xlab="Threads", ylim=c(0,35),
     ylab="Time (s)", main="2 4000x4000 matrices" )
lines(threads26$n, threads26$time, col="blue", type="b", pch=19 )
abline( h=8.16, lty=2 )
dev.off()

pdf( "time-matlevel.pdf", width=5, height=5 )
plot(threads14$n, threads14$time, type="b", pch=19,
     xlab="Threads", 
     ylab="Time (s)", main="1 4000x4000 matrix" )
abline( h=4, lty=2 )
dev.off()
