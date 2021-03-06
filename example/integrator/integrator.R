d <- data.frame(read.table("output/integrator.dat"))
names(d) <- c("t","n1")
attach(d)
pdf( "output/integrator.pdf", height=5, width=5 )
plot( t, n1, type="l", ylab="output", ylim=c(0,1) )
dev.off()
