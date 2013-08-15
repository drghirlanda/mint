program <- commandArgs(trailingOnly=TRUE)[1]
data <- read.table(paste("output/",program,".dat",sep=""))
pdf( paste("output/results-",program,".pdf",sep="") )
par( mfrow=c(2,1), oma=c(0,0,0,0), mar=c(4,4,0.6,1) )
plot( data[,1], data[,2], type="l",
     xlab="t", ylim=c(-0.1,1.1), ylab="" )
lines( data[,1], data[,3], col=2 )
legend( "bottomright", legend=c("energy","position"),
       lty=c(1,1), col=c(1,2) )
plot( data[,1], data[,4], type="l", ylab="activity",
     xlab="t", ylim=c(-0.1,1.1) )
lines( data[,1], data[,5], col=2 )
legend( "bottomright", legend=c("posphoto","negphoto"),
       lty=c(1,1), col=c(1,2) )

dev.off()
