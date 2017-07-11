setwd("././Excels")

#3
deltas <- read.csv("deltas.csv",header=FALSE)
plot(x=15:25,y=deltas[,1],type="l",col="red",main="Plot of Delta",ylab="Delta",xlab="S0")

gammas <- read.csv("gammas.csv",header=FALSE)
plot(x=15:25,y=gammas[,1],type="l",col="red",main="Plot of Gamma",ylab="Gamma",xlab="S0")

vegas <- read.csv("vegas.csv",header=FALSE)
plot(x=15:25,y=vegas[,1],type="l",col="red",main="Plot of Vega",ylab="Vega",xlab="S0")

thetas <- read.csv("thetas.csv",header=FALSE)
plot(x=15:25,y=thetas[,1],type="l",col="red",main="Plot of Theta",ylab="Theta",xlab="S0")

rhos <- read.csv("rhos.csv",header=FALSE)
plot(x=15:25,y=rhos[,1],type="l",col="red",main="Plot of Rho",ylab="Rho",xlab="S0")


#5
uniDist <- read.csv("uniformDist.csv",header=FALSE)
uniDist <- uniDist[,c(1,2)]
halton1 <- read.csv("HaltonSeq1.csv",header=FALSE)
halton2 <- read.csv("HaltonSeq2.csv",header=FALSE)

plot(x=uniDist[,1],y=uniDist[,2],xlab="Uniform Dist",ylab="Uniform Dist",main="Uniform distribution scatter")
plot(x=halton1[,1],y=halton1[,2],xlab="Halton1(base 2)",ylab="Halton2(base 7)",main="Halton Sequence 1 scatter")
plot(x=halton2[,1],y=halton2[,2],xlab="Halton1(base 2)",ylab="Halton2(base 4)",main="halton Sequence 2 scatter")


