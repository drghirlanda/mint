d <- read.table("output/deltarule.dat")
names(d) <- c("t","reward","out")
splus <- d[d$reward==1,]
sminus <- d[d$reward==0,]

pdf( "output/deltarule.pdf", height=5, width=5 )
plot(splus$t, splus$out, type="b", pch=19, xlab="t",
     ylab="output", ylim=c(0,1) )
lines( sminus$t, sminus$out, type="b", pch=1 )
legend("topleft",legend=c("S+","S-"), bty="n",
       pch=c(19,1) )
dev.off()
