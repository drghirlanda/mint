d <- read.table("deltarule.dat")
names(d) <- c("t","reward","out")
splus <- d[d$reward==1,]
sminus <- d[d$reward==0,]
pdf( "deltarule.pdf", height=5, width=5 )
plot(splus$t, splus$out, type="b", pch=15, xlab="t",
     ylab="output", ylim=c(0,1) )
lines( sminus$t, sminus$out, type="b", pch=3 )
legend("topleft",legend=c("S+","S-"), bty="n",
       pch=c(15,3) )
dev.off()
