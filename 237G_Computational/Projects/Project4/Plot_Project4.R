setwd("././Project4")
#Q1
q1 <- read.csv("Q1.csv",header=FALSE)
q1 <- q1[,c(1:7)]
par(mfrow=c(1,1))
xaxis <-c(10,20,40,80,100,200,500) 
plot(x=xaxis,y = q1[2,],main="Plot of convergence of call option pricing", type="l", ylab="Call Price", xlab="No of Steps")
lines(x=xaxis,y = q1[1,],col="red")
lines(x=xaxis,y = q1[3,],col="blue")
lines(x=xaxis,y = q1[4,],col="green")
legend("topright",c("Method 1","Method 2","Method 3","Method 4"),col=c("red","black","blue","green"),lty=1)

#Q2
library(xts)
q2 <- read.csv("Google_data.csv")
q2_xts <- xts(q2[,2],as.Date(q2[,1],"%m/%d/%Y"))
returns <- (q2_xts - lag(q2_xts))/(lag(q2_xts))
returns.yearly <- apply.yearly(1+returns,prod)-1
sd(returns.yearly,na.rm=T)

library(RQuantLib)
impVol <- bscallimpvol(916.44000200000005,1000,0.02,0.75-(8/252),0,23.9)

#Q3
q3 <- read.csv("Q3.csv",header=FALSE)
par(mfrow=c(3,2))
#a
plot(x=seq(20,80,2),y=q3[1,c(1:31)],type = "l", main="Delta as function of S0",xlab="S0",ylab="Delta")

#b
plot(x=seq(0,0.3846,0.01),y=q3[2,c(1:39)],type = "l", main="Delta as function of T", xlab="Time to expiration",ylab="Delta")

#c
plot(x=seq(20,80,2),y=q3[3,c(1:31)],type = "l", main="Theta as function of S0",xlab="S0",ylab="Theta")

#d
plot(x=seq(20,80,2),y=q3[4,c(1:31)],type = "l", main="Gamma as function of S0",xlab="S0", ylab="Gamma")

#e
plot(x=seq(20,80,2),y=q3[5,c(1:31)],type = "l", main="Vega as function of S0",xlab="S0", ylab="Vega")

#f
plot(x=seq(20,80,2),y=q3[6,c(1:31)],type = "l", main="Rho as function of S0", xlab = "S0", ylab = "Rho")

#Q4
par(mfrow=c(1,1))
q4 <- read.csv("Q4.csv",header=FALSE)
q4 <- q4[,c(1:11)]
plot(x=seq(80,120,4),y=q4[2,],main="Plot of European and American Put Option price", xlab="Current stock price", ylab="Put Price", type="l")
lines(x=seq(80,120,4),y=q4[1,],col="blue")
legend("topright",c("European","American"),col=c("blue","black"),lty=1)

#Q5
par(mfrow=c(1,1))
q5 <- read.csv("Q5.csv",header=FALSE)
q5 <- q5[,c(1:9)]
xaxis <- c(10,15,20,40,70,80,100,200,500)
plot(x=xaxis,y=q5[1,],main="Plot of trinomial call price", xlab="No of time steps", ylab="Call Price", type="l")
lines(x=xaxis,y=q5[2,],col="blue")
legend("bottomright",c("Stock Prices","Log Stock prices"),col=c("black","blue"),lty=1)

