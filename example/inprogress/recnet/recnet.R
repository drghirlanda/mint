d <- data.frame(read.table("output/recnet.dat"))
nodes <- dim(d)[2]-1
names(d) <- c("t",paste("n",seq(1,nodes),sep=""))
attach(d)
pdf( "output/recnet.pdf", height=5, width=5 )
plot( t, n1, type="l", ylab="output", ylim=c(0,1) )
for( i in seq(2,nodes) ) {
  lines( t, d[,i], col=i )
}
dev.off()
