suppressMessages(library(data.table))
suppressMessages(library(zoo))
suppressMessages(library(moments))

######
#Function to calculate sum of lagged bond marketcap, value weighted and equal weighted bond returns
######
PS2_Q1 <- function(CRSP_Bonds){
  
  data <- CRSP_Bonds
  
  #Clean up, change -99.0 to NA
  data <- data[TMRETNUA == -99.0,TMRETNUA:=NA]
  
  #Sort, get Month, Year, Lag of Outstanding
  setorder(CRSP_Bonds,MCALDT)
  data[,c("Month","Year"):=.(month(MCALDT),year(MCALDT))]
  data[,c("LagOut"):=.(shift(TMTOTOUT,1)),by=KYCRSPID]
  
  #Calculate Metrics
  #Not removing NA lagged outstanding, as we need it for equal weighted
  #TMTOTOUT is already in millions
  output <- data[,.(Bond_lag_MV = sum(LagOut,na.rm = TRUE),
                    Bond_Ew_Ret = sum(TMRETNUA,na.rm = TRUE)/length(!is.na(TMRETNUA)),
                    Bond_Vw_Ret = sum(TMRETNUA*LagOut, na.rm = TRUE)/sum(LagOut,na.rm = TRUE))
                 , by = .(Year,Month)]
  
  return(output[Year>=1926 & Year<=2015])
}

####
#Function to get excess value weighted stock and bond returns
####
PS2_Q2 <- function(Monthly_CRSP_Stocks,Monthly_CRSP_Bonds,Monthly_CRSP_Riskless){
  setorder(Monthly_CRSP_Stocks,Year,Month)
  
  #Riskfree rate
  Monthly_CRSP_Riskless[,c("Month","Year"):=.(month(caldt),year(caldt))]
  setorder(Monthly_CRSP_Riskless,Year,Month)
  
  #Merge all data table
  universe <- merge(Monthly_CRSP_Stocks,Monthly_CRSP_Riskless,by=c("Year","Month"))
  universe <- merge(universe,Monthly_CRSP_Bonds,by=c("Year","Month"))
  
  #Retrieve output
  output <- universe[,.(Year,Month,Stock_lag_MV,Stock_Excess_Vw_Ret = (Stock_Vw_Ret - t30ret),
                        Bond_lag_MV,Bond_Excess_Vw_Ret = (Bond_Vw_Ret - t30ret))]
  
  return(output[Year>=1926 & Year<=2015])
}

########
#Function to create value weighted, 60-40 weighted, Levered and Unlevered risk parity portfolio
########
PS2_Q3 <- function(Monthly_CRSP_Universe){
  
  data.full <- Monthly_CRSP_Universe
  setorder(data.full,Year,Month)
  
  #If first row is NaN
  if(data.full[1,"Bond_Excess_Vw_Ret"] == 'NaN'){
    data.full[1,"Bond_Excess_Vw_Ret"] <- NA  
  }
    
  #Calculate value weighted returns of bond and stock
  data.full[,Vw_weight := Stock_lag_MV/(Stock_lag_MV + Bond_lag_MV)]
  data.full[,Excess_Vw_Ret := Vw_weight * Stock_Excess_Vw_Ret + (1-Vw_weight)*Bond_Excess_Vw_Ret]
  
  #Calculate 60-40 portfolio of bond and stock
  data.full[,Excess_60_40_Ret := 0.6 * Stock_Excess_Vw_Ret + 0.4 * Bond_Excess_Vw_Ret]
  
  #Find Stock/Bond Invest sigma hat based on 3 year past rolling window (excluding current) 
  data.full[,c("Stock_inverse_sigma_hat","Bond_inverse_sigma_hat") := 
                          .(1/(shift(rollapply(data=Stock_Excess_Vw_Ret,FUN=sd,width=36,fill=NA,align="right"))),
                            1/(shift(rollapply(data=Bond_Excess_Vw_Ret,FUN=sd,width=36,fill=NA,align="right"))))]
  
  
  #Find k = 1/(stock sig)^-1 + (bond sig)^-1
  data.full[,Unlevered_k := 1/(Stock_inverse_sigma_hat + Bond_inverse_sigma_hat)] 
 
  #Find unlevered beta portfolio returns
  data.full[,Excess_Unlevered_RP_Ret := Unlevered_k*Stock_inverse_sigma_hat*Stock_Excess_Vw_Ret+
                                                    Unlevered_k*Bond_inverse_sigma_hat*Bond_Excess_Vw_Ret] 
  
  
  #Sd of value weighted is levered k
  K_Den <- sd((data.full$Stock_inverse_sigma_hat*data.full$Stock_Excess_Vw_Ret + 
                 data.full$Bond_inverse_sigma_hat*data.full$Bond_Excess_Vw_Ret),na.rm = TRUE)
  K <- sd(data.full$Excess_Vw_Ret,na.rm=TRUE)/K_Den
  data.full[,Levered_k := rep(K,nrow(data.full))]
  
  #Find levered beta portfolio returns
  data.full[,Excess_Levered_RP_Ret := Levered_k*Stock_inverse_sigma_hat*Stock_Excess_Vw_Ret+
                          Levered_k*Bond_inverse_sigma_hat*Bond_Excess_Vw_Ret] 
  
  return(data.full[,c("Year","Month","Stock_Excess_Vw_Ret","Bond_Excess_Vw_Ret",
                                  "Excess_Vw_Ret","Excess_60_40_Ret","Stock_inverse_sigma_hat",
                                  "Bond_inverse_sigma_hat","Unlevered_k","Excess_Unlevered_RP_Ret",
                                  "Levered_k","Excess_Levered_RP_Ret")])  
}


#######
#Function to calculate mean,sd, sharpe ratio, skewness and kurtosis of the various assets and portfolios
#######
PS2_Q4 <- function(Port_Rets){
  
  #Restrict data to data range in question
  Port_Rets <- Port_Rets[Year>=1929 & (Year<2010 | (Year==2010 & Month<=6))]
  
  answers <- matrix(ncol=6, nrow=6)
  row.names(answers) <- c("CRSP Stocks","CRSP Bonds","Value-weighted portfolio","60/40 portfolio","unlevered RP","levered RP")
  colnames(answers) <- c("Annualized Mean","t-stat of Annualized Mean","Annualized Standard Deviation","Annualized Sharpe Ratio",
                         "Skewness","Excess Kurtosis")
  answers["CRSP Stocks","Annualized Mean"] <- mean(Port_Rets$Stock_Excess_Vw_Ret,na.rm = TRUE)*12
  answers["CRSP Stocks","Annualized Standard Deviation"] <- sd(Port_Rets$Stock_Excess_Vw_Ret,na.rm = TRUE)*sqrt(12)
  answers["CRSP Stocks","Annualized Sharpe Ratio"] <- answers["CRSP Stocks","Annualized Mean"]/answers["CRSP Stocks","Annualized Standard Deviation"]
  answers["CRSP Stocks","Skewness"] <- skewness(Port_Rets$Stock_Excess_Vw_Ret,na.rm = TRUE)
  answers["CRSP Stocks","Excess Kurtosis"] <- kurtosis(Port_Rets$Stock_Excess_Vw_Ret,na.rm = TRUE)-3
  answers["CRSP Stocks","t-stat of Annualized Mean"] <- t.test(Port_Rets$Stock_Excess_Vw_Ret)$statistic
  
  answers["CRSP Bonds","Annualized Mean"] <- mean(Port_Rets$Bond_Excess_Vw_Ret,na.rm = TRUE)*12
  answers["CRSP Bonds","Annualized Standard Deviation"] <- sd(Port_Rets$Bond_Excess_Vw_Ret,na.rm = TRUE)*sqrt(12)
  answers["CRSP Bonds","Annualized Sharpe Ratio"] <- answers["CRSP Bonds","Annualized Mean"]/answers["CRSP Bonds","Annualized Standard Deviation"]
  answers["CRSP Bonds","Skewness"] <- skewness(Port_Rets$Bond_Excess_Vw_Ret,na.rm = TRUE)
  answers["CRSP Bonds","Excess Kurtosis"] <- kurtosis(Port_Rets$Bond_Excess_Vw_Ret,na.rm = TRUE)-3
  answers["CRSP Bonds","t-stat of Annualized Mean"] <- t.test(Port_Rets$Bond_Excess_Vw_Ret)$statistic
  
  answers["Value-weighted portfolio","Annualized Mean"] <- mean(Port_Rets$Excess_Vw_Ret,na.rm = TRUE)*12
  answers["Value-weighted portfolio","Annualized Standard Deviation"] <- sd(Port_Rets$Excess_Vw_Ret,na.rm = TRUE)*sqrt(12)
  answers["Value-weighted portfolio","Annualized Sharpe Ratio"] <- 
    answers["Value-weighted portfolio","Annualized Mean"]/answers["Value-weighted portfolio","Annualized Standard Deviation"]
  answers["Value-weighted portfolio","Skewness"] <- skewness(Port_Rets$Excess_Vw_Ret,na.rm = TRUE)
  answers["Value-weighted portfolio","Excess Kurtosis"] <- kurtosis(Port_Rets$Excess_Vw_Ret,na.rm = TRUE)-3
  answers["Value-weighted portfolio","t-stat of Annualized Mean"] <- t.test(Port_Rets$Excess_Vw_Ret)$statistic
  
  answers["60/40 portfolio","Annualized Mean"] <- mean(Port_Rets$Excess_60_40_Ret,na.rm = TRUE)*12
  answers["60/40 portfolio","Annualized Standard Deviation"] <- sd(Port_Rets$Excess_60_40_Ret,na.rm = TRUE)*sqrt(12)
  answers["60/40 portfolio","Annualized Sharpe Ratio"] <- answers["60/40 portfolio","Annualized Mean"]/answers["60/40 portfolio","Annualized Standard Deviation"]
  answers["60/40 portfolio","Skewness"] <- skewness(Port_Rets$Excess_60_40_Ret,na.rm = TRUE)
  answers["60/40 portfolio","Excess Kurtosis"] <- kurtosis(Port_Rets$Excess_60_40_Ret,na.rm = TRUE)-3
  answers["60/40 portfolio","t-stat of Annualized Mean"] <- t.test(Port_Rets$Excess_60_40_Ret)$statistic
  
  answers["unlevered RP","Annualized Mean"] <- mean(Port_Rets$Excess_Unlevered_RP_Ret,na.rm = TRUE)*12
  answers["unlevered RP","Annualized Standard Deviation"] <- sd(Port_Rets$Excess_Unlevered_RP_Ret,na.rm = TRUE)*sqrt(12)
  answers["unlevered RP","Annualized Sharpe Ratio"] <- answers["unlevered RP","Annualized Mean"]/answers["unlevered RP","Annualized Standard Deviation"]
  answers["unlevered RP","Skewness"] <- skewness(Port_Rets$Excess_Unlevered_RP_Ret,na.rm = TRUE)
  answers["unlevered RP","Excess Kurtosis"] <- kurtosis(Port_Rets$Excess_Unlevered_RP_Ret,na.rm = TRUE)-3
  answers["unlevered RP","t-stat of Annualized Mean"] <- t.test(Port_Rets$Excess_Unlevered_RP_Ret)$statistic
  
  answers["levered RP","Annualized Mean"] <- mean(Port_Rets$Excess_Levered_RP_Ret,na.rm = TRUE)*12
  answers["levered RP","Annualized Standard Deviation"] <- sd(Port_Rets$Excess_Levered_RP_Ret,na.rm = TRUE)*sqrt(12)
  answers["levered RP","Annualized Sharpe Ratio"] <- answers["levered RP","Annualized Mean"]/answers["levered RP","Annualized Standard Deviation"]
  answers["levered RP","Skewness"] <- skewness(Port_Rets$Excess_Levered_RP_Ret,na.rm = TRUE)
  answers["levered RP","Excess Kurtosis"] <- kurtosis(Port_Rets$Excess_Levered_RP_Ret,na.rm = TRUE)-3
  answers["levered RP","t-stat of Annualized Mean"] <- t.test(Port_Rets$Excess_Levered_RP_Ret)$statistic
  
  #Formatting
  #mean,sd to % and 2 decimals
  answers[,"Annualized Mean"] <- round(answers[,"Annualized Mean"],4)*100
  answers[,"Annualized Standard Deviation"] <- round(answers[,"Annualized Standard Deviation"],4)*100
  
  answers[,c("Annualized Sharpe Ratio","Skewness","Excess Kurtosis","t-stat of Annualized Mean")] <- 
    round(answers[,c("Annualized Sharpe Ratio","Skewness","Excess Kurtosis","t-stat of Annualized Mean")],2)
  
  return(answers)
}
