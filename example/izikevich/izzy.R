library(STAR)

d <- data.frame(read.table("output/izzy.dat"))

pdf( "output/izzy.pdf", height=5, width=10 )
plot( d[,1], d[,2], type="l", xlab="input", ylab="v" )
abline( h=30, col="red" )
grid( col="black" )
dev.off()

last <- dim(d)[2]

pdf( "output/izzyraster.pdf" )
raster(apply( d[,2:last]>=30, 2, which ), main="", ylab="node",
       xlab="time (ms)" )
dev.off()

pdf( "output/izzysum.pdf" )
plot(apply( d[,2:last]>=30, 1, sum ), type="l",
     xlab="time (ms)", ylab="Total Activity (spikes)")
dev.off()
