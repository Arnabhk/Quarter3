library(knitr)
#Q1
setwd("C:/_UCLA/Quarter3/237G_Computational/Projects/Project6/Project4")
callPrices <- read.csv("callPrices.csv",header = FALSE)
plot(x=seq(0.12,0.48,0.04),y=callPrices[,1],type="l",main="Lookback call prices",ylab="Call Price",xlab="Volatility")

putPrices <- read.csv("putPrices.csv",header = FALSE)
plot(x=seq(0.12,0.48,0.04),y=putPrices[,1],type="l",main="Lookback put prices",ylab="Put Price",xlab="Volatility")

#Q2
allPrices.orig <- allPrices
allPrices <- read.csv("AllAnswers.csv",header=FALSE)
allPrices <- allPrices[,c(1:6)]
colnames(allPrices) <- c("lambda1","lambda2","Time","OptionPrice","DefaultTime","Probability")

#fix lambda1
fix.lambda1 <- unique(allPrices[allPrices$lambda1 == 0.2,])
fix.lambda2 <- unique(allPrices[allPrices$lambda2 == 0.4,])

#a
colors <- c("black","blue","red","yellow","green","brown","pink","purple","orange")
par(mfrow=c(2,1))
plot(y=fix.lambda1[fix.lambda1$lambda2 == 0.0,"OptionPrice"],x=fix.lambda1[fix.lambda1$lambda2 == 0.0,"Time"],main="Graph for Option price, lambda1=0.2",xlab="Time",ylab="OptionPrice",type="l",ylim=c(0,6000))
colCount <- 1
for(val in seq(0.0,0.8,0.1)){
  lines(y=fix.lambda1[fix.lambda1$lambda2 == val,"OptionPrice"],x=fix.lambda1[fix.lambda1$lambda2 == val,"Time"],col=colors[colCount])
  colCount <-  colCount + 1
}

plot(y=fix.lambda2[fix.lambda2$lambda1 == 0.05,"OptionPrice"],x=fix.lambda2[fix.lambda2$lambda1 == 0.05,"Time"],main="Graph for Option price, lambda2=0.4",xlab="Time",ylab="OptionPrice",type="l",ylim=c(3000,5500))
colCount <- 1
for(val in seq(0.05,0.4,0.05)){
  lines(y=fix.lambda2[fix.lambda2$lambda1 == val,"OptionPrice"],x=fix.lambda2[fix.lambda2$lambda1 == val,"Time"],col=colors[colCount])
  colCount <- colCount + 1
}


#b
plot(y=fix.lambda1[fix.lambda1$lambda2 == 0.1,"DefaultTime"],x=fix.lambda1[fix.lambda1$lambda2 == 0.1,"Time"],main="Graph for Default Time, lambda1=0.2",xlab="Time",ylab="DefaultTime",type="l",ylim=c(0.5,1.7))
colCount <- 1
for(val in seq(0.1,0.8,0.1)){
  lines(y=fix.lambda1[fix.lambda1$lambda2 == val,"DefaultTime"],x=fix.lambda1[fix.lambda1$lambda2 == val,"Time"],col=colors[colCount])
  colCount <-  colCount + 1
}

plot(y=fix.lambda2[fix.lambda2$lambda1 == 0.05,"DefaultTime"],x=fix.lambda2[fix.lambda2$lambda1 == 0.05,"Time"],main="Graph for Default Time, lambda2=0.4",xlab="Time",ylab="DefaultPrie",type="l",ylim=c(0.8,1.5))
colCount <- 1
for(val in seq(0.05,0.4,0.05)){
  lines(y=fix.lambda2[fix.lambda2$lambda1 == val,"DefaultTime"],x=fix.lambda2[fix.lambda2$lambda1 == val,"Time"],col=colors[colCount])
  colCount <- colCount + 1
}


#c
plot(y=fix.lambda1[fix.lambda1$lambda2 == 0.0,"Probability"],x=fix.lambda1[fix.lambda1$lambda2 == 0.0,"Time"],main="Graph for Probability, lambda1=0.2",xlab="Time",ylab="Probability",type="l",ylim=c(0.2,1))
colCount <- 1
for(val in seq(0.0,0.8,0.1)){
  lines(y=fix.lambda1[fix.lambda1$lambda2 == val,"Probability"],x=fix.lambda1[fix.lambda1$lambda2 == val,"Time"],col=colors[colCount])
  colCount <-  colCount + 1
}

plot(y=fix.lambda2[fix.lambda2$lambda1 == 0.05,"Probability"],x=fix.lambda2[fix.lambda2$lambda1 == 0.05,"Time"],main="Graph for Probability, lambda2=0.4",xlab="Time",ylab="Probability",type="l",ylim=c(0.7,1))
colCount <- 1
for(val in seq(0.05,0.4,0.05)){
  lines(y=fix.lambda2[fix.lambda2$lambda1 == val,"Probability"],x=fix.lambda2[fix.lambda2$lambda1 == val,"Time"],col=colors[colCount])
  colCount <- colCount + 1
}



#plot(y=fix.lambda1$DefaultTime,x=fix.lambda1$lambda2,main="Graph for default time, lambda1=0.2,Time=5",xlab="lambda2",ylab="Default time",type="l")
#plot(y=fix.lambda2$DefaultTime,x=fix.lambda2$lambda1,main="Graph for default time, lambda2=0.4,Time=5",xlab="lambda1",ylab="Default time",type="l")

#plot(y=fix.lambda1$Probability,x=fix.lambda1$lambda2,main="Graph for probability, lambda1=0.2,Time=5",xlab="lambda2",ylab="Probability",type="l")
#plot(y=fix.lambda2$Probability,x=fix.lambda2$lambda1,main="Graph for probability, lambda2=0.4,Time=5",xlab="lambda1",ylab="Probability",type="l")



###Testing
jumps <- read.csv("Jumps.csv",header=FALSE)
jumps <- jumps[,c(1:60)]

jg <- apply(jump.indicator,1,function(x){which(x==1)[1]})
nr <- apply(jumps,1,function(x){which(x==1)[1]})

sum(!is.na(jg))/length(jg)
sum(!is.na(nr))/length(nr)

coll <- read.csv("CollPaths.csv",header=FALSE)
coll <- coll[,c(1:60)]

matplot(t(Vt),x=c(1:61),type="l")
matplot(t(coll),x=c(1:60),type="l")

plot(y=as.data.frame(t(Vt)),x=c(1:61),type="l")
coll[1,]
