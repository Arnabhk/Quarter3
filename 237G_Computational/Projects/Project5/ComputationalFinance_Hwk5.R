#####NITISH RAMKUMAR

#####QUESTION 1

###Functions to build the independent variables
BuildIndependentVariables <- function(X,type,k){
  if(type=="Hermite"){
    return(BuildHermite(X,k))
  }
  else if(type=="Laguerre"){
    return(BuildLaguerre(X,k))
  }
  else if(type=="Monomial"){
    return(BuildMonomial(X,k))
  }
}

BuildHermite <- function(X,k){
  output <- matrix(nrow=length(X),ncol=k)
  for(it in 1:k){  
    if(it == 1){
      output[,it] <- 1
    }
    else if(it == 2){
      output[,it] <- 2*X
    }
    else{
      output[,it] <- 2*X*output[,it-1] - 2*(it-1)*output[,it-2] 
    }
  }
  return(output)
}

BuildLaguerre <- function(X,k){
  apply(matrix(c(1:k),nrow=1),2,function(it){
    if(it==1){
      output <- exp(-X/2)
    }
    else if(it==2){
      output <- exp(-X/2) *(1-X)
    }
    else if(it==3){
      output <- exp(-X/2)*(1 - 2*X + (X^2/2))
    }
    else if(it==4){
      output <- exp(-X/2)*(1 - 3*X + 3*(X^2)/2 - (X^3)/6)
    }
    else if(it==5){
      output <- exp(-X/2)*(1 - 4*X + 3*(X^2) - 2*(X^3)/3 + (X^4)/24)
    }
    output
  })
}

BuildMonomial <- function(X,k){
  apply(matrix(c(1:k),nrow=1),2,function(it){
    X^(it-1)
  })
}

####Function to build stock path
BuildStockPath <- function(s0,Paths,Steps,time){

  #Get randomValues
  randVals <- matrix(rnorm(Paths*Steps/2),ncol=Steps)
  stockPaths <- matrix(nrow=Paths,ncol=Steps)
  dt <- time/Steps
  
  #Have a matrix with increasing time
  temp <- t(apply(matrix(rep(1,Paths*Steps/2),nrow=Steps),2,cumsum))
  
  #normal
  stockPaths[c(1:(Paths/2)),] <- s0 * exp((r - (sigma^2)/2)*dt*temp + sigma * sqrt(dt) * t(apply(randVals,1,cumsum)))
  
  #antithetic
  stockPaths[c(((Paths/2)+1):Paths),] <- s0 * exp((r - (sigma^2)/2)*dt*temp + sigma * -1 * sqrt(dt)*t(apply(randVals,1,cumsum)))
  
  return(stockPaths)
}


####Function to perform LSMC
PerformLSMC <- function(stockPaths,sigma,r, strike, Paths,Steps,time, k,type){
  optionPayoff <- rep(NA,Paths)
  exerciseStep <- rep(0,Paths)
  dt <- time/Steps
  
  #Calculate price at T
  optionPayoff <- pmax(strike-stockPaths[,Steps],0)
  
  #store step where exercising is valid. 
  #For first iteration, set it to last step if the payoff is positive
  exerciseStep[which(optionPayoff>0)] <- Steps
  
  #Now move back and calculate price at each point
  for(stepCount in (Steps-1):1){
    #Exercise Price for this step  
    exercisePrice <- pmax(strike-stockPaths[,stepCount],0)
    
    ###Continuation price###
    #Find all itm paths
    ITMPaths <- which(exerciseStep != 0)
    #Find the k X terms for the regression
    RegXVal <- stockPaths[ITMPaths,stepCount]
    RegX <- BuildIndependentVariables(X=RegXVal,type = type,k = k)
    
    #Set the Y term for the regression
    RegY <- exp(-r * dt * (exerciseStep[ITMPaths]-stepCount)) * optionPayoff[ITMPaths]
    
    #Perform regression
    out <- lm(RegY~RegX-1)
    
    #Calculate expected continuation price
    continuePrice <- rep(0,length(exercisePrice))
    continuePrice[ITMPaths] <- RegX%*%out$coefficients
    
    #Perform check if exercise happens here
    exerciseThisStep <- (exercisePrice >= continuePrice) & (exercisePrice!=0)
    
    #update payoff
    optionPayoff[exerciseThisStep] <- exercisePrice[exerciseThisStep]
    exerciseStep[exerciseThisStep] <- stepCount
  }
  
  Price0.All <- exp(-r * exerciseStep * dt) * optionPayoff
  Price0 <- mean(Price0.All)
  
  return(Price0)
}


###TESTING - QUESTION1########
set.seed(1234)
strike <- 40
sigma <- 0.2
r <- 0.06
Paths <- 100000
Steps <- 50
time <- 0.5 

k <- c(2,3,4)
startVal <- c(36,40,44)
timeVal <- c(0.5,1,2)
typeVal <- c("Laguerre","Hermite","Monomial")
answers <- expand.grid(k,startVal,timeVal,typeVal)
colnames(answers) <- c("K","S0","Time","Type")

answers$Price <- mapply(function(S0,Time,K,Type){
  stockPaths <- BuildStockPath(S0,Paths,Steps,Time) 
  PerformLSMC(stockPaths,sigma,r, strike, Paths,Steps,Time, K,Type)
},answers$S0,answers$Time,answers$K,answers$Type)


#####Part a###########
answers[answers$Type=="Laguerre",c(1,2,3,5)]

#####Part b###########
answers[answers$Type=="Hermite",c(1,2,3,5)]

#####Part c##########
answers[answers$Type=="Monomial",c(1,2,3,5)]

####Comparison (time)####
k <- 4
s0 <- 40
strike <- 40
Paths <- 100000
Steps <- 50
Time <- 1

timetaken <- rep(NA,3)

stockPaths_timeCheck <- BuildStockPath(s0,Paths,Steps,Time)
lagu_start <- proc.time()
lagu_price <- PerformLSMC(stockPaths_timeCheck,sigma,r, strike, Paths,Steps,Time, k,"Laguerre") 
lagu_end <- proc.time()
timetaken[1] <- (lagu_end - lagu_start)[3]

hermite_start <- proc.time()
hermite_price <- PerformLSMC(stockPaths_timeCheck,sigma,r, strike, Paths,Steps,Time, k,"Hermite") 
hermite_end <- proc.time()
timetaken[2] <- (hermite_end - hermite_start)[3]

mon_start <- proc.time()
mon_price <- PerformLSMC(stockPaths_timeCheck,sigma,r, strike, Paths,Steps,Time, k,"Monomial") 
mon_end <- proc.time()
timetaken[3] <- (mon_end - mon_start)[3]

names(timetaken) <- c("Laguerre","Hermite","Monomial")
timetaken


#####QUESTION 2
set.seed(1234)
s0 <- 65
strike <- 60
sigma <- 0.2
r <- 0.06
Paths <- 100000
Steps <- 50
time <- 1
strikeTime <- 0.2

stockPaths_2 <- BuildStockPath(s0,Paths,Steps,time)

#European
strikeStep <- (strikeTime/time)*Steps
eur_forward_optPrices <- pmax(stockPaths_2[,strikeStep] - stockPaths_2[,Steps],0) * exp(-r*time)
eur_forward_optPrice <- mean(eur_forward_optPrices)
eur_forward_optPrice

k <- 4
type <- "Monomial"
amr_forward_optPrice <- PerformLSMC(stockPaths_2[,c(strikeStep:ncol(stockPaths_2))],sigma,r, stockPaths_2[,strikeStep], Paths,Steps-strikeStep, time, k,type)
amr_forward_optPrice <- amr_forward_optPrice*exp(-r*strikeStep*(time/Steps))
amr_forward_optPrice

