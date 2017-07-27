### Quant Asset Management Problem Set 3
### Nitish Ramkumar - 704873047

suppressMessages(library(data.table))
suppressMessages(library(zoo))
suppressMessages(library(moments))

#Function to compute the momentum universe
PS3_Q1 <- function(CRSP_Stocks){
  #Filter Share code of 10,11
  #Filter Exchange Code of 1,2,3
  CRSP_Stocks <- CRSP_Stocks[SHRCD %in% c(10,11) & EXCHCD %in% c(1,2,3)]
  
  #If return is -99.0, -88.0,-77.0, -66.0, B, C for RET
  CRSP_Stocks[RET %in% c("B","C","-99","-88","-77","-66",""," "), RET:=NA]
  
  #If delist return is -99.0, -88.0,-77.0, -66.0, P, S, T
  CRSP_Stocks[DLRET %in% c("A","P","S","T","-99","-88","-77","-66",""," "),DLRET := NA]
  
  #MarketCap and remove when price and marketcap lag is missing?
  setorder(CRSP_Stocks,date)
  CRSP_Stocks[,MktCap := abs(PRC) * abs(SHROUT)]
  CRSP_Stocks[,lag_Mkt_Cap := shift(MktCap)/1000, by=PERMNO]
  
  #Convert to numeric and create necessary columns
  CRSP_Stocks[,RET:=as.numeric(as.character(RET))]
  CRSP_Stocks[,DLRET:=as.numeric(as.character(DLRET))]
  CRSP_Stocks[,c("Month","Year") := .(month(date),year(date))]
  CRSP_Stocks[,c("NumMonths") := .((Year*12)+Month)]
  
  #fill in missing months
  CRSP_Stocks[, TimeDiff := as.integer(NumMonths - shift(NumMonths)), by = PERMNO] 
  Incomplete_Stocks <-  unique(CRSP_Stocks[TimeDiff != 1, .(PERMNO)]) 
  setkey(Incomplete_Stocks, PERMNO)
  Time <-  seq(from = min(CRSP_Stocks$NumMonths, na.rm = T), to = max(CRSP_Stocks$NumMonths, na.rm = T))
  filled <-  Incomplete_Stocks[CJ(unique(Incomplete_Stocks$PERMNO), Time), allow.cartesian = T, roll = -Inf] # Build a datatable with all possible times
  CRSP_Stocks <-  merge(CRSP_Stocks, filled, by.x = c('PERMNO', 'NumMonths'), by.y = c('PERMNO', 'V2'), all = T) # Merge in the filled dataset into the original
  setorder(CRSP_Stocks,NumMonths,PERMNO)
  
  #Delist returns
  CRSP_Stocks <- CRSP_Stocks[,RET := ifelse(is.na(DLRET),RET,ifelse(is.na(RET),DLRET,(1+RET)*(1+DLRET) - 1))]
  
  #Calculate cumulative returns
  #Set CUMRET = NA, if less than 8 months of returns are available
  CRSP_Stocks[,RET_2 := !is.na(shift(RET,2)), by = PERMNO]
  CRSP_Stocks[,ValidMonths := rollapplyr(RET_2, 11, function(x){sum(x,na.rm=TRUE)}, fill=NA, partial=TRUE), by = PERMNO]
  CRSP_Stocks[,c("Valid13Prc","ValidRet2"):=.(!is.na(shift(PRC,13)),!is.na(shift(RET,2))),PERMNO]
  CRSP_Stocks[,Ranking_Ret := rollapplyr(log(shift(1+RET,2)), 11, sum, fill=NA, partial = TRUE), PERMNO]
  CRSP_Stocks[!Valid13Prc | !ValidRet2 | is.na(lag_Mkt_Cap) | ValidMonths < 8, Ranking_Ret := NA]

  setnames(CRSP_Stocks,"RET","Ret")
  output <- CRSP_Stocks[,c("Year","Month","PERMNO","EXCHCD","lag_Mkt_Cap","Ret","Ranking_Ret")]
  setorder(output,PERMNO,Year,Month)
  return(output[Year >= 1927 & Year <= 2015, ])
}

#Function to calculate KRF and DM deciles
PS3_Q2 <- function(CRSP_Stocks_Momentum){
  #Remove NA for Ranking_ret before sorting
  CRSP_Stocks_Momentum_decile <- CRSP_Stocks_Momentum
  CRSP_Stocks_Momentum_decile <- CRSP_Stocks_Momentum_decile[!is.na(Ranking_Ret)]
  
  #Sort based on DM deciles
  CRSP_Stocks_Momentum_decile[,DM_decile := as.numeric(cut(Ranking_Ret,
                                                           breaks = quantile(Ranking_Ret,probs=seq(0,1,by=0.1),type = 7),
                                                           include.lowest = T)),
                              .(Year,Month)]
  #Sort based on FF deciles
  CRSP_Stocks_Momentum_decile[,KRF_decile := as.numeric(cut(Ranking_Ret,
                                                            breaks = quantile(.SD[EXCHCD==1,Ranking_Ret],
                                                                              probs=seq(0,1,by=0.1),type = 7),
                                                            include.lowest = T)),
                              .(Year,Month)]
  
  #The cut doesnt catch values less than least value and greater than highest value
  #Based on a check, for all these values, the DM decile is 1 or 10 (worst or best).
  #So we can just set that directly
  CRSP_Stocks_Momentum_decile[is.na(KRF_decile) ,KRF_decile := DM_decile]
  
  CRSP_Stocks_Momentum_decile$DM_decile <- as.integer(CRSP_Stocks_Momentum_decile$DM_decile)
  CRSP_Stocks_Momentum_decile$KRF_decile <- as.integer(CRSP_Stocks_Momentum_decile$KRF_decile)
  return(CRSP_Stocks_Momentum_decile[,c("Year","Month","PERMNO","lag_Mkt_Cap","Ret","DM_decile","KRF_decile")])
}


#Function to calculate the decile returns for KRF and DM
PS3_Q3 <- function(CRSP_Stocks_Momentum_decile,FF_mkt){
  #Restrict FF data for the necessary years
  FF_mkt <- FF_mkt[Year>=1927 & Year<=2015]
  
  #Split DM and KRF for value weight calculation
  DM <- CRSP_Stocks_Momentum_decile[,c("Year","Month","PERMNO","lag_Mkt_Cap","Ret","DM_decile")]
  KRF <- CRSP_Stocks_Momentum_decile[,c("Year","Month","PERMNO","lag_Mkt_Cap","Ret","KRF_decile")]
  
  #Calculate VW seperately
  DM_dec <- DM[,.(DM_Ret <-  sum(lag_Mkt_Cap*Ret, na.rm = T)/(sum(lag_Mkt_Cap,na.rm=T))), .(Year,Month,DM_decile)]
  setorder(DM_dec, Year, Month, DM_decile)
  
  KRF_dec <- KRF[,.(KRF_Ret <-  sum(lag_Mkt_Cap*Ret, na.rm = T)/(sum(lag_Mkt_Cap,na.rm=T))), .(Year,Month,KRF_decile)]
  setorder(KRF_dec, Year, Month, KRF_decile)
  
  #Merge together and with FF Rf information
  mergeVal <- merge(DM_dec,KRF_dec,by.x = c("Year","Month","DM_decile"),by.y=c("Year","Month","KRF_decile"))
  setkey(mergeVal,Year,Month)
  setkey(FF_mkt,Year,Month)
  
  CRSP_Stocks_Momentum_returns <- merge(mergeVal,FF_mkt,by = c("Year","Month"))
  setnames(CRSP_Stocks_Momentum_returns,"V1.x","DM_Ret")
  setnames(CRSP_Stocks_Momentum_returns,"V1.y","KRF_Ret")
  setnames(CRSP_Stocks_Momentum_returns,"DM_decile","decile")
  setorder(CRSP_Stocks_Momentum_returns,Year,Month,decile)
  return(CRSP_Stocks_Momentum_returns[,c("Year","Month","decile","DM_Ret","KRF_Ret","Rf")])
}


#Function to calculate moments for DM returns
PS3_Q4 <- function(CRSP_Stocks_Momentum_returns){
  #Paper mentions the time period for calculations
  returns <- CRSP_Stocks_Momentum_returns[Year>=1927 & (Year < 2013 | (Year == 2013 & Month <=3))]
  output <- matrix(nrow=4,ncol=11)
  
  #Calculate moments for deciles
  means_dec <- returns[,.(mean = mean((DM_Ret-Rf)*100,na.rm=T)*12),.(decile)]
  sds_dec <- returns[,.(sd = sd(DM_Ret,na.rm=T)*sqrt(12)*100),.(decile)]
  skm_dec <- returns[,.(skm=skewness(log(1+DM_Ret))),.(decile)]
  sr_dec <- means_dec/sds_dec
  
  #Calculate Winner minus loser portfolio and its moments
  wl <- returns[decile==10,DM_Ret*100] - returns[decile==1,DM_Ret*100] 
  wl_excess <- wl
  
  mean_wl <- mean(wl_excess)*12  
  sd_wl <- sd(wl_excess)*sqrt(12)
  sr_wl <- mean_wl/sd_wl
  sk_wl <- skewness(log(1 + (wl/100) + returns[decile==1,Rf])) 
  
  
  #Set output and format names and decimal digits 
  output[1,] <- c(means_dec$mean,mean_wl)
  output[2,] <- c(sds_dec$sd,sd_wl)
  output[3,] <- c(sr_dec$mean,sr_wl)
  output[4,] <- c(skm_dec$skm,sk_wl)
  
  colnames(output) <- c("Dec1","Dec2","Dec3","Dec4","Dec5","Dec6","Dec7","Dec8","Dec9","Dec10","WML")
  row.names(output) <- c("Mean Excess Return","Std","Sharpe Ratio","Skewness")
  
  output[1,] <- round(output[1,],1)
  output[2,] <- round(output[2,],1)
  output[3,] <- round(output[3,],2)
  output[4,] <- round(output[4,],2)
  
 return(output)
}

#Function to calculate correlation with actual DM and KRF returns
PS3_Q5 <- function(CRSP_Stocks_Momentum_returns,DM_returns,KRF_returns){
  
  returns <- CRSP_Stocks_Momentum_returns[Year>=1927 & Year<=2015]
  setorder(returns,Year,Month,decile)
  
  dm_returns <- DM_returns[Year>=1927 & Year<=2015]
  setorder(dm_returns,Year,Month,decile)
  
  krf_returns <- KRF_returns[Year>=1927 & Year <=2015]
  setorder(krf_returns,Year,Month,decile)
  
  output <- matrix(nrow=2,ncol=11)
  
  for(count in 1:10){
    port_dm_ret <- returns[decile == count,DM_Ret]
    dm_ret <- dm_returns[decile == count,DM_Ret]
    output[1,count] <- cor(port_dm_ret,dm_ret)
    
    port_krf_ret <- returns[decile == count,KRF_Ret]
    krf_ret <- krf_returns[decile == count,KRF_Ret]
    output[2,count] <- cor(port_krf_ret,krf_ret)
  }
   
  wl_port_dm <- returns[decile==10,DM_Ret] - returns[decile==1,DM_Ret]
  wl_dm <- dm_returns[decile == 10,DM_Ret] - dm_returns[decile == 1,DM_Ret]
  output[1,11] <- cor(wl_port_dm,wl_dm)
    
  wl_port_krf <- returns[decile==10,KRF_Ret] - returns[decile==1,KRF_Ret]
  wl_krf <- krf_returns[decile == 10,KRF_Ret] - krf_returns[decile == 1,KRF_Ret]
  output[2,11] <- cor(wl_port_krf,wl_krf)
  
  colnames(output) <- c("Dec1","Dec2","Dec3","Dec4","Dec5","Dec6","Dec7","Dec8","Dec9","Dec10","WML")
  row.names(output) <- c("DM Correlation","KRF Correlation")
  
  output <- round(output,4)
  
  return(output)
}

