d <- read.table("feature-reversal.dat")
names(d) <- c("t","cs","out")
splus <- subset( d, cs=="A" )
sminus <- subset( d, cs=="X" )
pdf( "feature-reversal.pdf", height=5, width=5 )
plot(splus$t, splus$out, type="l", xlab="t",
     ylab="output", ylim=c(0,1) )
lines( sminus$t, sminus$out, col=2 )
legend("topleft",legend=c("A","X"), bty="n",
       lty=1, col=c(1,2) )
dev.off()
