d <- data.frame(read.table("recnet.dat"))
names(d) <- c("t","output")
attach(d)
pdf( "recnet.pdf", height=5, width=5 )
plot( t, output, type="l", xlab="time", ylab="output", ylim=c(0,1),
     xaxs="i", las=1, yaxs="i" )
grid( col="black" )
dev.off()
