#LIBRARIES
suppressMessages(require(data.table))
suppressMessages(require(dplyr))
suppressMessages(require(moments))
suppressMessages(require(lubridate))

#Function to calculate monthly value weight and equal weighted returns of the market portfolio 
PS1_Q1 <-  function(CRSP_Stocks){
  #RETURN CALCULATION
  ##US Common Stocks in NYSE,AMEX and NASDAQ
  ##Also remove entries which have both RET and DLRET = NA
  #CRSP_Stocks <- stockInfo
  CRSP_Stocks <- CRSP_Stocks[SHRCD %in% c(10,11) & EXCHCD %in% c(1,2,3) & !(is.na(RET) & is.na(DLRET))]
  CRSP_Stocks <- CRSP_Stocks[!RET %in% c(-99,-88,-77,-66,-55,-44)]
  CRSP_Stocks <- CRSP_Stocks[!DLRET %in% c(-99,-88,-77,-66,-55,-44)]
  
  ##Annualize returns and handle delist
  #convert to log returns
  CRSP_Stocks <- CRSP_Stocks[,c('RET','DLRET','RETX','DLRETX'):= .(as.numeric(as.character(RET)),
                                                                   as.numeric(as.character(DLRET)),
                                                                   as.numeric(as.character(RETX)),
                                                                   as.numeric(as.character(DLRETX)))]
  CRSP_Stocks[,c('month','year','RET','DLRET','RETX','DLRETX'):=
                               .(month(DATE),year(DATE),log(1+RET),log(1+DLRET),
                                 log(1+RETX),log(1+DLRETX))] 
  
  #annualize (if data is daily) by adding within groups of year and month
  ret <- CRSP_Stocks[,.(ret=sum(RET,na.rm = TRUE),delistret=sum(DLRET,na.rm = TRUE)),
                     by=.(PERMNO,year,month)]
  
  #Delist handling
  #The previous line would have made all NA delist return = 0
  #else we need to add log returns of month before and after delist 
  ret[,tret:=exp(ret+delistret)-1]
  
  
  ##MARKETCAP
  CRSP_Stocks[,mcap:=abs(PRC)*abs(SHROUT)*1000]
  mcapcrsp <- CRSP_Stocks[,.(PERMNO,DATE,month,year,mcap)]
  #in case data in daily, take last instance of marketcap
  stkmcap <- mcapcrsp[,.(mcap=last(mcap),DATE=last(DATE)),by=.(PERMNO,year,month)] 
  fulldata <- merge(ret,stkmcap,by = c("PERMNO","year","month"))
  #store lag of market cap
  fulldata[,mcaplag:=c(NA, mcap[-.N]),by=c("PERMNO")]
    
  
  #OUTPUT METRICS
  ##1 - lag market cap sum
  mcapRes <- fulldata[,.(mcaplagsum = sum(mcaplag,na.rm = TRUE)/1000000),by=.(year,month)]
  
  ##2 - Equal weighted returns
  equalW <- fulldata[,.(equalW = sum(tret)/length(tret)),by=.(year,month)]
  
  ##3 - Value weighted returns
  valueW <- fulldata[,.(valW = sum(tret*mcaplag,na.rm = TRUE)/sum(mcaplag,na.rm = TRUE)),by=.(year,month)]
  
  finalResult <- merge(mcapRes,merge(valueW,equalW,by = c("month","year")),by = c("year","month"))
  colnames(finalResult) <- c("Year","Month","Stock_lag_MV","Stock_Vw_Ret","Stock_Ew_Ret")
  return(finalResult)
}

#Get statistics of FF market portfolio and constructed market portfolio
PS1_Q2 <- function(Monthly_CRSP_Stocks,FF_Mkt){
  results <- matrix(nrow = 5,ncol=2)
  row.names(results) <- c("Mean","Standard Deviation","Sharpe Ratio","Skewness","Excess Kurtosis")
  colnames(results) <- c("Estimated","Actual")

  totalData <- merge(Monthly_CRSP_Stocks,FF_Mkt,by = c("Year","Month"))
  #remove risk free rate from constructed returns
  totalData[,Stock_Vw_Ret:=Stock_Vw_Ret-Rf]
  
  results["Mean","Estimated"] <- (1+mean(totalData$Stock_Vw_Ret,na.rm = TRUE))^12 - 1 
  results["Mean","Actual"] <- (1+mean(totalData$Market_Minus_Rf,na.rm = TRUE))^12 - 1
  
  results["Standard Deviation","Estimated"] <- sd(totalData$Stock_Vw_Ret,na.rm = TRUE)*sqrt(12)
  results["Standard Deviation","Actual"] <- sd(totalData$Market_Minus_Rf,na.rm = TRUE)*sqrt(12)
  
  results["Sharpe Ratio","Estimated"] <- results["Mean","Estimated"]/results["Standard Deviation","Estimated"]
  results["Sharpe Ratio","Actual"] <- results["Mean","Actual"]/results["Standard Deviation","Actual"]
  
  results["Skewness","Estimated"] <- skewness(totalData$Stock_Vw_Ret,na.rm = TRUE)
  results["Skewness","Actual"] <- skewness(totalData$Market_Minus_Rf,na.rm = TRUE)
  
  results["Excess Kurtosis","Estimated"] <- kurtosis(totalData$Stock_Vw_Ret,na.rm = TRUE)-3
  results["Excess Kurtosis","Actual"] <- kurtosis(totalData$Market_Minus_Rf,na.rm = TRUE)-3
  
  return(results)
}

PS1_Q3 <- function(Monthly_CRSP_Stocks,FF_Mkt){
  
  totalData <- merge(Monthly_CRSP_Stocks,FF_Mkt,by = c("Year","Month"))
  totalData[,Stock_Vw_Ret:=Stock_Vw_Ret-Rf]
  
  #Get correlation
  corVal <- cor(totalData$Stock_Vw_Ret,totalData$Market_Minus_Rf,use="na.or.complete")
  totalData[,diff:=abs(Stock_Vw_Ret-Market_Minus_Rf)]
  setorder(totalData,diff)
  #Get max diff
  maxDif <- max(totalData$diff,na.rm=TRUE)
  results <- c(corVal,maxDif)
  names(results) <- c("Correlation","Max difference")
  return(results)
}  


