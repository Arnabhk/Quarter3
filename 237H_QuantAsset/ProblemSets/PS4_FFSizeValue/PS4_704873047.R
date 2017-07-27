#########NITISH RAMKUMAR
######### PROBLEM SET 4

library(knitr)
suppressMessages(library(data.table))
suppressMessages(library(zoo))
library(moments)
setwd("C:/_UCLA/Quarter3/237H_QuantAsset/ProblemSets/PS4")

###LINK TO WRDS
setupWRDS <- function(){
  suppressMessages(library(rJava))
  options(java.parameters = '-Xmx4g')
  suppressMessages(library(RJDBC))
  
  sasPath <- "C:\\Users\\nitis\\OneDrive\\Documents\\WRDS_Drivers"
  sasCore <- paste0(sasPath, "\\sas.core.jar")
  sasDriver <- paste0(sasPath, "\\sas.intrnet.javatools.jar")
  user <- "nramkuma"
  pass <- "{SAS002}AF665B391E73D5D3492960545A7E616B"
  .jinit()
  .jaddClassPath(c(sasCore, sasDriver))
  drv <- JDBC("com.sas.net.sharenet.ShareNetDriver", sasDriver, identifier.quote="")
  wrds <<- dbConnect(drv, "jdbc:sharenet://wrds-cloud.wharton.upenn.edu:8551/", user, pass)
}

# Function to get data #
getData <- function(sql, n = -1){
  #setup connection
  res <- dbSendQuery(wrds, sql)
  dbHasCompleted(res)
  
  #perform fetch
  returnData <- fetch(res, n)
  
  #clear memory
  dbClearResult(res)
  return(returnData)
}

#Data Retrieval
setupWRDS()

#Get stock returns and price data from CRSP
crsp.data <- read.csv("StocksData_New.csv",stringsAsFactors=T,header=T)
crsp.data <- data.table(crsp.data)
CRSP_Stocks <- copy(crsp.data)

#Get compustat info
cmpst.data <- fread("Compustat.csv",stringsAsFactors = T,showProgress = F)
Compustat_Stocks <- copy(cmpst.data)

#Get prba info
cmpst.prba <- fread("PRBA.csv",stringsAsFactors = T, showProgress = F)
Compustat_prba <- copy(cmpst.prba)

#Link data
sql.permnoLink <- "SELECT GVKEY,LPERMNO,LPERMCO,LINKTYPE,LINKPRIM, LINKDT,LINKENDDT,USEDFLAG FROM CRSPA.CCMXPF_LINKTABLE"
permnoLink <- getData(sql.permnoLink)
permnoLink <- data.table(permnoLink)
linkData <- copy(permnoLink)

#CRSP
#Function to clean CRSP and add missing rows
cleanCRSPStocksAndAddMissingRows <- function(CRSP_Stocks){
  
  #Filter Share code of 10,11
  #Filter Exchange Code of 1,2,3
  CRSP_Stocks <- CRSP_Stocks[SHRCD %in% c(10,11) & EXCHCD %in% c(1,2,3)]
  
  #If return is -99.0, -88.0,-77.0, -66.0, B, C for RET
  CRSP_Stocks[RET %in% c("B","C","-99","-88","-77","-66",""," "), RET:=NA]
  #If delist return is -99.0, -88.0,-77.0, -66.0, P, S, T
  CRSP_Stocks[DLRET %in% c("A","P","S","T","-99","-88","-77","-66",""," "),DLRET := NA]
  
  #CRSP NAICS for financials
  CRSP_Stocks <- CRSP_Stocks[!grepl("^52",NAICS)]
  
  #MarketCap 
  setorder(CRSP_Stocks,date)
  CRSP_Stocks[,MktCap := abs(PRC) * abs(SHROUT)/1000]
  
  #format fields and compute new fields for month, year and num months until today
  CRSP_Stocks[,date := as.Date(as.character(date),"%Y%m%d")]
  CRSP_Stocks[,RET:=as.numeric(as.character(RET))]
  CRSP_Stocks[,DLRET:=as.numeric(as.character(DLRET))]
  CRSP_Stocks[,c("Month","Year") := .(month(date),year(date))]
  CRSP_Stocks[,c("NumMonths") := .((Year*12)+Month)]
  
  #Fill missing months
  CRSP_Stocks[, TimeDiff := as.integer(NumMonths - shift(NumMonths)), by = .(PERMNO,PERMCO)] 
  Incomplete_Stocks <-  unique(CRSP_Stocks[TimeDiff != 1, .(PERMNO,PERMCO,k=1)]) 
  setkey(Incomplete_Stocks, PERMNO,PERMCO)
  Time <-  seq(from = min(CRSP_Stocks$NumMonths, na.rm = T), to = max(CRSP_Stocks$NumMonths, na.rm = T))
  Time <- data.table(Time)[,k:=1]
  filled <- merge(Incomplete_Stocks,Time,by = "k",allow.cartesian = T)
  CRSP_Stocks <-  merge(CRSP_Stocks, filled, by.x = c('PERMNO','PERMCO','NumMonths'), by.y = c('PERMNO','PERMCO','Time'), all = T) 
  setorder(CRSP_Stocks,PERMNO,PERMCO,NumMonths)
  
  #Delist returns
  CRSP_Stocks <- CRSP_Stocks[,RET := ifelse(is.na(DLRET),RET,ifelse(is.na(RET),DLRET,(1+RET)*(1+DLRET) - 1))]
  length(unique(CRSP_Stocks[Year==2015 & Month==12, PERMNO]))
  
  return(CRSP_Stocks)
}
CRSP_Stocks <- cleanCRSPStocksAndAddMissingRows(CRSP_Stocks)

#COMPUSTAT
#Function clean compustat and merge with PRBA 
cleanupCompustat <- function(Compustat_Stocks,Compustat_prba){
  
  #remove financial companies
  Compustat_Stocks <- Compustat_Stocks[datafmt=="STD" & indfmt=="INDL"]
  Compustat_prba <- Compustat_prba[indfmt=="INDL"]
  
  #Merge with PRBA based on gvkey and date
  setkey(Compustat_Stocks,gvkey,datadate)
  setkey(Compustat_prba,gvkey,datadate)
  Compustat_merged <- merge(Compustat_Stocks,Compustat_prba,all.x = TRUE)
  Compustat_merged[,datadate:=as.Date(as.character(datadate),"%Y%m%d")]
  
  ##Calculate BE
  Compustat_merged <- calculateBEValue(Compustat_merged)
  
  return(Compustat_merged)
}
#Function used by cleanCompustat to calculate BE
calculateBEValue <- function(Compustat_merged){
  #Calcuate SHE
  Compustat_merged[,SHE:=ifelse(!is.na(seq),seq,ifelse(!is.na(ceq+pstk),
                                                       ceq+pstk,ifelse(!is.na(mib),at-lt-mib,at-lt)))]
  
  #Calculate DT
  Compustat_merged[,DT:=ifelse(!is.na(txditc),txditc,NA)]
  Compustat_merged[is.na(DT),DT:=rowSums(.SD,na.rm = T),.SDcols=c("itcb","txdb")]
  
  #Calculate PS
  Compustat_merged[,PS:=ifelse(!is.na(pstkrv),pstkrv,ifelse(!is.na(pstkl),pstkl,pstk))]
  
  #Create column with NA for PS and PRBA and add for every row
  Compustat_merged[,c("NegPS","NegPRBA"):=.(-PS,-prba)]
  Compustat_merged[,BE:=rowSums(.SD,na.rm = T),.SDcols=c("SHE","NegPS","DT","NegPRBA")]
  Compustat_merged[is.na(SHE),BE:=NA]

  return(Compustat_merged)
}
Compustat_merged <- cleanupCompustat(Compustat_Stocks,Compustat_prba)

#Function to merge Compustat and linkData
performMerge <- function(linkData,Compustat_merged){
  linkData[,c("gvkey","lpermno","lpermco"):=.(as.numeric(gvkey),as.integer(lpermno),as.integer(lpermco))]
  #Remove everything other than LU, LC and LS, and primary type = P,C
  #Also remove rows which dont fit into the linkdate range
  linkData <- linkData[! linktype%in% c("LD","NR","NU","NP","LX","LN","LS") & linkprim %in% c("P","C")]
  setkey(linkData,gvkey)
  setkey(Compustat_merged,gvkey)
  compustat_link <- merge(Compustat_merged,linkData,all.x = T,allow.cartesian = T)
  compustat_link <- compustat_link[datadate>=linkdt & (datadate<=linkenddt | is.na(linkenddt))]
  setkey(compustat_link,lpermno,fyear)
  
  return(compustat_link)
}
compustat_link <- performMerge(linkData,Compustat_merged)

#SIZE
#Function to get ranking based on size for both deciles and SMB/HML
getSizeRanking <- function(stockInfo,isFactor){
  #Accumulate the marketcap within each company
  stockInfo_co <- stockInfo[,.(MktCap=sum(MktCap,na.rm=T),EXCHCD=last(EXCHCD)),.(PERMCO,Year,Month)]
  
  #make sure there is valid marketcap at june of the year
  sizeRank_Info <- stockInfo_co[Month == 6,.(Year,MktCap,PERMCO,EXCHCD)]
  
  #Change number of breakpoints based on whether it is SMB or not.
  if(isFactor){
    breakprobs = c(0.0,0.5,1)
    maxRanking = 2
  } else{
    breakprobs = seq(0,1,0.1)
    maxRanking = 10
  }
  
  #Perform ranking
  sizeRank_Info[,min := quantile(.SD[EXCHCD==1 & !is.na(MktCap),MktCap],probs=breakprobs,type = 7)[1],Year]
  sizeRank_Info[,SizeRank := as.numeric(cut(MktCap,breaks = quantile(.SD[EXCHCD==1 & !is.na(MktCap),MktCap],breakprobs,type = 7),
                                            include.lowest = T)),.(Year)]
  sizeRank_Info[is.na(SizeRank),SizeRank:=ifelse(MktCap<=min,1,maxRanking)]
  setkey(sizeRank_Info,PERMCO,Year)
  setkey(stockInfo,PERMCO,Year)
  
  #Merge returns with rank
  sizeRank_Full <- merge(stockInfo,sizeRank_Info,all.x = T)
  setnames(sizeRank_Full,c("MktCap.x","EXCHCD.x"),c("MktCap","EXCHCD"))
  
  #Need to move if forward by 6 months due to the time of rebalancing within a year
  sizeRank_Full[,c("Act_Size_Rank","Lag_MktCap") := .(shift(SizeRank,6),shift(MktCap,1)),.(PERMNO)]
  sizeRank_Full <- sizeRank_Full[!is.na(Act_Size_Rank) & !is.na(Lag_MktCap) & Lag_MktCap != 0]
  setorder(sizeRank_Full,Year,Month,Act_Size_Rank)
  return(sizeRank_Full)
}

#funtion to get Size Deciles returns
getSizeDeciles <- function(CRSP_Stocks){
  Size_Ranking <- getSizeRanking(CRSP_Stocks,FALSE)
  
  #Calculate value weighted return
  Size_Ret <- Size_Ranking[,.(Size_Ret=sum(Lag_MktCap*RET,na.rm = T)/sum(Lag_MktCap,na.rm=T)),
                           .(Year,Month,Act_Size_Rank)]
  setorder(Size_Ret,Year,Month,Act_Size_Rank)
  Size_output <- Size_Ret[Year>=1973 & Year<=2015]
  setkey(Size_output,Year,Month)
  return(Size_output)
}

Size_output <- getSizeDeciles(CRSP_Stocks)

#BTM
#Function to get ranking based on BEME for both deciles and SMB/HML
getBeMeRanking <- function(stockInfo,isFactor){
  #Accumulate within a company
  stockInfo_co <- stockInfo[,.(MarketCap=sum(MktCap,na.rm=T),EXCHCD=last(EXCHCD)),.(PERMCO,Year,Month)]
  
  annual_stock <- stockInfo_co[Month==12,.(MarketCap,EXCHCD,PERMCO,Year)]
  setkey(annual_stock,PERMCO,Year)
  
  #Use fyear due to difference in datadate and fyear
  ranking_beme <- merge(annual_stock,compustat_link,by.x=c('PERMCO','Year'),by.y=c('lpermco','fyear'))
  #####BE/ME ranking
  ranking_beme <- ranking_beme[,.(PERMCO,Year,gvkey,MarketCap,BE,EXCHCD,at)]
  setkey(ranking_beme,PERMCO,Year)
  
  ###Use historical BE
  hist_be_btm <- histBe[,c(-2,-3)]
  hist_be_btm <- melt(hist_be_btm,id.vars = c("PERMNO"))
  colnames(hist_be_btm) <- c("PERMNO","Year","HistBe")
  hist_be_btm$Year <- as.numeric(as.character(hist_be_btm$Year))
  ranking_beme <- merge(ranking_beme,hist_be_btm,by.x = c("PERMCO","Year"),by.y=c("PERMNO","Year"),all.x=T)
  ranking_beme <- ranking_beme[is.na(BE) & !is.na(HistBe) & HistBe!=-99.99, BE:=HistBe]
  
  #Few checks
  ranking_beme[is.na(BE) | BE==0 | (!isFactor & is.na(at)),BE:=NA]
  ranking_beme[is.na(MarketCap) | MarketCap==0,MarketCap:=NA]
  
  #Calculate BE/ME
  ranking_beme[,BEME:=BE/MarketCap]
  
  #2 year check
  #if(isFactor){
  #  ranking_beme[,hist_id:=seq_len(.N), by=PERMCO]
  #  ranking_beme[hist_id<=2,BEME:=NA]  
  #}
  
  #Ranking based on BE/ME
  if(isFactor){
    breakprob = c(0,0.3,0.7,1)
    maxRanking = 3
  } else{
    breakprob = seq(0,1,0.1)
    maxRanking = 10
  }
  
  ranking_beme[,min := quantile(.SD[EXCHCD==1 & !is.na(BEME) & (!isFactor | BEME>=0),BEME],probs=breakprob,type = 7)[1],Year]
  ranking_beme[,BEMERank := as.numeric(cut(BEME,breaks = quantile(.SD[EXCHCD==1 & !is.na(BEME) & (!isFactor | BEME>=0),BEME],
                                                                  probs=breakprob,type = 7),
                                           include.lowest = T)),.(Year)]
  
  ranking_beme[is.na(BEMERank),BEMERank:=ifelse(is.na(BEME) | (isFactor & BEME<0),NA,ifelse(BEME<=min,1,maxRanking))]
  setorder(ranking_beme,PERMCO,Year)
  setkey(ranking_beme,PERMCO,Year)
  
  #Shift rank forward by one year as we are using it for next year (another 6 month move forward later)
  ranking_beme[,BEMERank_lead := shift(BEMERank),by=PERMCO]
  setkey(stockInfo,PERMCO,Year)
  #merge with BEME ranking
  Final_BEME <- merge(stockInfo,ranking_beme,all.x=T)
  setnames(Final_BEME,"EXCHCD.x","EXCHCD")
  setorder(Final_BEME,PERMCO,Year,Month)
  
  #shift Rank foward by 6 as rank is valid only for july. Shift marketcap forward by 1 as we need lag marketcap
  Final_BEME[,c("Act_BEME_Rank","Lag_MktCap") := .(shift(BEMERank_lead,6),shift(MktCap,1)),.(PERMNO)]
  #Remove all situations where market cap is NA or 0
  Final_BEME <- Final_BEME[!is.na(Act_BEME_Rank) & !is.na(Lag_MktCap) & Lag_MktCap!=0.0]
  setorder(Final_BEME,PERMNO,Year,Month)
  return(Final_BEME)
}

#Function to get Value deciles returns
getValueDeciles <- function(CRSP_Stocks){
  Beme_Ranking <- getBeMeRanking(CRSP_Stocks,FALSE)
  #Calculate value weighted
  Beme_Ret <- Beme_Ranking[,.(Beme_Ret=sum(Lag_MktCap*RET,na.rm = T)/sum(Lag_MktCap,na.rm=T)),
                           .(Year,Month,Act_BEME_Rank)]
  setorder(Beme_Ret,Year,Month,Act_BEME_Rank)
  Beme_output <- Beme_Ret[Year>=1973 & Year<=2015]
  setkey(Beme_output,Year,Month)
  
  return(Beme_output)
}

#BEME
Beme_output <- getValueDeciles(CRSP_Stocks)

#Get HML, SMB by independent sorting
getSMBHMLPort <- function(CRSP_Stocks){
  
  #Get ranks from both size and BTM independently
  bemeRank <- getBeMeRanking(CRSP_Stocks,TRUE)
  sizeRank <- getSizeRanking(CRSP_Stocks,TRUE)
  
  bemeRank <- bemeRank[,.(PERMNO,PERMCO,Year,Month,RET,Act_BEME_Rank,Lag_MktCap)]
  sizeRank <- sizeRank[,.(PERMNO,PERMCO,Year,Month,RET,Act_Size_Rank,Lag_MktCap)]
  setkey(bemeRank,PERMCO,PERMNO,Year,Month)
  setkey(sizeRank,PERMCO,PERMNO,Year,Month)
  
  #Merge the two and calculate a common rank with both. 2*BTMRank + SizeRank - 2
  size_beme <- merge(sizeRank,bemeRank)    
  size_beme[,portCount:=(2*Act_BEME_Rank) + Act_Size_Rank - 2]
  
  size_beme_ret <- size_beme[,.(ret=sum(Lag_MktCap.x*RET.x,na.rm =T)/sum(Lag_MktCap.x,na.rm=T)),
                             .(Year,Month,portCount)]
  setorder(size_beme_ret,Year,Month,portCount)
  
  #Calculate HML and SMB returns
  size_beme_final <- size_beme_ret[,.(PortRet_HML = 0.5*(.SD[portCount==5,ret]+.SD[portCount==6,ret]-
                                                           .SD[portCount==1,ret] - .SD[portCount==2,ret]),
                                      PortRet_SMB = (.SD[portCount==1,ret]+.SD[portCount==3,ret]+.SD[portCount==5,ret]-
                                                       .SD[portCount==2,ret]-.SD[portCount==4,ret]-.SD[portCount==6,ret])/3),.(Year,Month)]
  
  hml_smb_output <- size_beme_final[Year>=1973 & Year<=2015,.(Year,Month,PortRet_HML,PortRet_SMB)]
  setorder(hml_smb_output,Year,Month)
  
  return(hml_smb_output)
}
SMB_HML <- getSMBHMLPort(CRSP_Stocks)
HMLOutput <- SMB_HML[,c(1,2,3)]
SMBOutput <- SMB_HML[,c(1,2,4)]


#################OUTPUTS##############################################
kable(Beme_output[Year==2015 & Month==12],col.names = c("Year","Month","port","BtM_Ret"),caption="Btm Deciles returns")
kable(Size_output[Year==2015 & Month==12],col.names = c("Year","Month","port","Size_Ret"),caption="Size Deciles returns")
kable(SMB_HML[Year==2015],caption="HMB and SML portfolios",col.names = c("Year","Month","HML_Ret","SMB_Ret"))


#COMPARISONS - SIZE
getFFDataDeciles <- function(excel_data,rf){
  excel_data <- excel_data[,c(1,11:20)]
  excel_data <- data.table(excel_data)
  rf_data <- data.table(rf)
  
  excel_data[ ,X:=as.numeric(X)]
  rf_data[,X:=as.numeric(X)]
  excel_data[,c("Year","Month"):=.(as.integer(X/100),X%%100)]
  rf_data[,c("Year","Month"):=.(as.integer(X/100),X%%100)]
  
  excel_data <- excel_data[Year>=1973 & Year<=2015]
  rf_data <- rf_data[Year>=1973 & Year<=2015]
  
  setorder(excel_data,Year,Month)
  setorder(rf_data,Year,Month)
  
  data_ff <- data.table()
  for(count in c(1:10)){
    data <- excel_data[,count+1,with=FALSE]/100
    full_data <- cbind(excel_data[,Year],excel_data[,Month],count,data,rf_data[,2]/100)
    colnames(full_data) <- c("Year","Month","Rank","Ret","Rf")
    data_ff <- rbind(data_ff,full_data)
  }
  setorder(data_ff,Year,Month,Rank)
}

hml_smb <- read.csv("F-F_Research_Data_Factors.csv",skip=3,nrow=1089)
size <- read.csv("Portfolios_Formed_on_ME.csv",skip=12,nrows=1089)
rf <- hml_smb[,c(1,5)]
size_ff <- getFFDataDeciles(size,rf)

size_data <- matrix(nrow=5,ncol=11,dimnames = list(c("Return","Standard Deviation",
                                                     "Sharpe Ratio","Skewness","Correlation"),c(1:10,"LongShort")))
for(i in c(1:10)){
  size_data[1,i] <- mean(Size_output[Act_Size_Rank==i,Size_Ret]-size_ff[Rank==i,Rf])*12
  size_data[2,i] <- sd(Size_output[Act_Size_Rank==i,Size_Ret])*sqrt(12)
  size_data[3,i] <- size_data[1,i]/size_data[2,i]
  size_data[4,i] <- skewness(Size_output[Act_Size_Rank==i,Size_Ret])
  size_data[5,i] <- cor(size_ff[Rank==i,Ret],Size_output[Act_Size_Rank==i,Size_Ret])
}

longshort_size <- Size_output[Act_Size_Rank==1,Size_Ret] - Size_output[Act_Size_Rank==10,Size_Ret]
size_data[1,11] <- mean(longshort_size)*12
size_data[2,11] <- sd(longshort_size)*sqrt(12)
size_data[3,11] <- size_data[1,11]/size_data[2,11]
size_data[4,11] <- skewness(longshort_size)
size_data[5,11] <- cor(longshort_size,(size_ff[Rank==1,Ret]-size_ff[Rank==10,Ret]))

kable(size_data[,1:6], caption="Size sort returns for first 6 deciles")
kable(size_data[,7:11], caption="Size sort returns for last 4 deciles and long short") 



#COMPARISONS -BTM

beme <- read.csv("Portfolios_Formed_on_BE-ME.csv",skip=23,nrows=1089)
beme_ff <- getFFDataDeciles(beme,rf)

btm_data <- matrix(nrow=5,ncol=11,dimnames = list(c("Return","Standard Deviation",
                                                    "Sharpe Ratio","Skewness","Correlation"),c(1:10,"LongShort")))
for(i in c(1:10)){
  btm_data[1,i] <- mean(Beme_output[Act_BEME_Rank==i,Beme_Ret]-beme_ff[Rank==i,Rf])*12
  btm_data[2,i] <- sd(Beme_output[Act_BEME_Rank==i,Beme_Ret])*sqrt(12)
  btm_data[3,i] <- btm_data[1,i]/btm_data[2,i]
  btm_data[4,i] <- skewness(Beme_output[Act_BEME_Rank==i,Beme_Ret])
  btm_data[5,i] <- cor(beme_ff[Rank==i,Ret],Beme_output[Act_BEME_Rank==i,Beme_Ret])
}

longshort_btm <- Beme_output[Act_BEME_Rank==10,Beme_Ret] - Beme_output[Act_BEME_Rank==1,Beme_Ret]
btm_data[1,11] <- mean(longshort_btm)*12
btm_data[2,11] <- sd(longshort_btm)*sqrt(12)
btm_data[3,11] <- btm_data[1,11]/btm_data[2,11]
btm_data[4,11] <- skewness(longshort_btm)
btm_data[5,11] <- cor(longshort_btm,(beme_ff[Rank==10,Ret]-beme_ff[Rank==1,Ret]))

kable(btm_data[,1:6], caption="Book-to-Market sort returns for first 6 deciles")
kable(btm_data[,7:11], caption="Book-to-Market sort returns for last 4 deciles and long short") 


#COMPARISONS - HML/SMB
hml_smb <- hml_smb[,c(1,3,4)]
hml_smb <- data.table(hml_smb)
hml_smb <- hml_smb[ ,X:=as.numeric(X)]
hml_smb <- hml_smb[,c("Year","Month"):=.(as.integer(X/100),X%%100)]
hml_smb <- hml_smb[Year>=1973 & Year<=2015]
rf_info <- data.table(rf)
rf_info <- rf_info[,X:=as.numeric(X)]

setorder(rf_info,X)
setorder(hml_smb,Year,Month)

hml_port <- hml_smb[,.(Year,Month,HML=HML/100)]
rf_info <- rf_info[X>=197301 & X<=201512, .(X = X,RF = RF/100)]
setorder(hml_port,Year,Month)

hml_smb_ans <- matrix(nrow=6,ncol=2,dimnames=list(c("Excess Return","Standard Deviation",
"Sharpe Ratio","Skewness","Correlation","Excess Return - Rf"),c("HML","SMB")))
hml_smb_ans[1,1] <- mean(HMLOutput[,PortRet_HML])*12
hml_smb_ans[2,1] <- sd(HMLOutput[,PortRet_HML])*sqrt(12)
hml_smb_ans[3,1] <- hml_smb_ans[1,1]/hml_smb_ans[2,1]
hml_smb_ans[4,1] <- skewness(HMLOutput[,PortRet_HML])
hml_smb_ans[5,1] <- cor(hml_port[,HML],HMLOutput[,PortRet_HML])
hml_smb_ans[6,1] <- mean(HMLOutput[,PortRet_HML]-rf_info$RF)*12

smb_port <- hml_smb[,.(Year,Month,SMB=SMB/100)]
setorder(smb_port,Year,Month)

hml_smb_ans[1,2] <- mean(SMBOutput[,PortRet_SMB])*12
hml_smb_ans[2,2] <- sd(SMBOutput[,PortRet_SMB])*sqrt(12)
hml_smb_ans[3,2] <- hml_smb_ans[1,2]/hml_smb_ans[2,2]
hml_smb_ans[4,2] <- skewness(SMBOutput[,PortRet_SMB])
hml_smb_ans[5,2] <- cor(smb_port[,SMB],SMBOutput[,PortRet_SMB])
hml_smb_ans[6,2] <- mean(SMBOutput[,PortRet_SMB]-rf_info$RF)*12

kable(hml_smb_ans)



