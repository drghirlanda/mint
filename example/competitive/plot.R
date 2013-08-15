data <- read.table("output/time.dat")
pdf( "output/time.pdf" )
my.color <- 1
T <- 1:1500
plot( data[T,1], data[T,2], type="l", col=my.color,
     xlab="t", ylab="activity", ylim=c(0,1) )
for( i in seq(3,dim(data)[2]) ) {
  my.color <- my.color + 1
  lines( data[T,1], data[T,i], col=my.color )
}
dev.off()

data <- read.table("output/tuning.dat")
pdf( "output/tuning.pdf" )
my.color <- 1
X <- 1:8
plot( X, data[,1], type="l", col=my.color,
     xlab="position", ylab="activity", ylim=c(0,1) )
for( i in seq(2,dim(data)[2]) ) {
  my.color <- my.color + 1
  lines( X, data[,i], col=my.color )
}
dev.off()
