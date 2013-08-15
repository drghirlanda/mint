fann <- data.frame( read.table( "fann/fann.dat", head=T ) )
mint <- data.frame( read.table( "mint/mint.dat", head=T ) )
pybrain <- data.frame( read.table( "pybrain/pybrain.dat", head=T ) )

pdf( "time.pdf", height=4, width=4 )
par( pch=16 )
plot( fann$N, fann$time/mint$time, type="o", col=1, log="xy",
     xlab="Size of input and hidden layers (N)", xlim=c(500,20000),
     ylab="Execution Time Ratio", ylim=c(1,25) )
points( pybrain$N, pybrain$time/mint$time, type="o", col=2 )
legend( "topright", legend=c("fann/mint", "pybrain/mint"),
       col=c(1,2), lty=c(1,1), bty="n" )
dev.off()

pdf( "memory.pdf", height=4, width=4 )
par( pch=16 )
plot( fann$N, fann$memory/mint$memory, type="b", col=1, ylim=c(1,20),
     xlab="Size of input and hidden layers (N)", xlim=c(500,20000),
     ylab="Memory Use Ratio", log="xy" )
points( pybrain$N, pybrain$memory/mint$memory, type="b", col=2 )
legend( "bottomleft", legend=c("fann/mint", "pybrain/mint"),
       col=c(1,2), lty=c(1,1), bty="n" )
dev.off()

pdf( "pagefaults.pdf", height=4, width=4 )
par( pch=16 )
plot( fann$N, fann$minor/mint$minor, type="b", col=1, ylim=c(1,20),
     xlab="Size of input and hidden layers (N)", xlim=c(500,20000),
     ylab="Minor Page Faults Ratio", log="xy" )
points( pybrain$N, pybrain$minor/mint$minor, type="b", col=2 )
legend( 1500, 9, legend=c("fann/mint", "pybrain/mint"),
       col=c(1,2), lty=c(1,1), bty="n" )
dev.off()
