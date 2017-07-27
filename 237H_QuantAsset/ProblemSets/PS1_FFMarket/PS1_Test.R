
readfromFile = TRUE

if(!readfromFile){
  #WRDS CONNECTION##############################################
  library(rJava)
  options(java.parameters = '-Xmx4g')
  library(RJDBC)
  
  sasPath <- "C:\\Users\\nitis\\OneDrive\\Documents\\WRDS_Drivers"
  sasCore <- paste0(sasPath, "\\sas.core.jar")
  sasDriver <- paste0(sasPath, "\\sas.intrnet.javatools.jar")
  .jinit()
  .jaddClassPath(c(sasCore, sasDriver))
  drv <- JDBC("com.sas.net.sharenet.ShareNetDriver", sasDriver, identifier.quote="")
  wrds <- dbConnect(drv, "jdbc:sharenet://wrds-cloud.wharton.upenn.edu:8551/", user, pass)
  
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
  ###########################################################
  
  setwd("C:/_UCLA/Quarter3/237H_QuantAsset/ProblemSets/PS1")
  #LIBRARIES
  require(data.table)
  
  #GET DATA - Q1
  sql1 <- "SELECT DATE,PERMNO,RET,RETX, PRC, SHROUT FROM CRSPQ.MSF WHERE DATE between '01feb1926'd and '31dec2015'd"
  sql2 <- "SELECT PERMNO, NAMEENDT, NAMEDT, SHRCD, EXCHCD  FROM CRSPQ.DSENAMES WHERE NAMEENDT >= mdy(01,02,1926) AND NAMEDT <= mdy(12,31,2015)"
  sql3 <- "SELECT PERMNO,DLSTDT,DLSTCD,NWPERM,DLRET,DLRETX FROM CRSPQ.DSEDELIST WHERE DLSTDT between '01feb1926'd and '31dec2015'd"
  stocks <- getData(sql1)
  stocknames <- getData(sql2)
  delist <- getData(sql3)
  
  #FORMAT DATA
  stocks$DATE <- as.Date(stocks$DATE)
  stocknames$NAMEDT <- as.Date(stocknames$NAMEDT)
  stocknames$NAMEENDT <- as.Date(stocknames$NAMEENDT)
  delist$DLSTDT <- as.Date(delist$DLSTDT)
  
  #Stock returns info in data table
  stocks <- data.table(stocks)
  stocks[,c("month","year"):=.(month(DATE),year(DATE))]
  setkey(stocks,PERMNO)
  setorder(stocks,PERMNO,DATE)
  
  #Put EXCHCD and SHRCD information into data table
  stocknames <- data.table(stocknames)
  setorder(stocknames,PERMNO)
  stocknames[,c("nameMnth","nameYr","namEndMnth","namEndYr"):=.(month(NAMEDT),year(NAMEDT),month(NAMEENDT),year(NAMEENDT))]
  setkey(stocknames,PERMNO)

  tret <- stocknames[stocks,nomatch=0,allow.cartesian=TRUE]
  tret <- tret[((nameYr < year) | (nameYr == year & nameMnth < month)) & ((namEndYr > year) | (namEndYr == year & namEndMnth >= month))]
  setorder(tret,PERMNO,DATE)
  tret <- tret[,c("NAMEDT","NAMEENDT","nameMnth","nameYr","namEndMnth","namEndYr"):=NULL]

  #Delist info in data table
  delist <- data.table(delist)
  delist <- delist[!is.na(DLSTDT) & DLSTCD!=100,.(PERMNO,DLSTDT,DLSTCD,NWPERM,DLRET,DLRETX)]
  delist[,c("month","year"):=.(month(DLSTDT),year(DLSTDT))]
  delist[,c("DLSTDT","DLSTCD","NWPERM"):=NULL]
  setkey(delist,PERMNO,month,year)
  
  #merge stock and delist info
  setkey(tret,PERMNO,year,month)
  stockInfo <- merge(delist,tret,by = c("PERMNO","year","month"),all = TRUE)
  setorder(stockInfo,PERMNO,DATE)
  stockInfo <- stockInfo[,.(PERMNO,DATE,SHRCD,EXCHCD,RET,RETX,DLRET,DLRETX,PRC,SHROUT)]
  
  str(stockInfo)
  stockInfo$PERMNO <- as.integer(stockInfo$PERMNO)
  stockInfo$SHRCD <- as.integer(stockInfo$SHRCD)
  stockInfo$EXCHCD <- as.integer(stockInfo$EXCHCD)
  stockInfo$SHROUT <- as.integer(stockInfo$SHROUT)
  stockInfo$RET <- as.factor(stockInfo$RET)
  stockInfo$RETX <- as.factor(stockInfo$RETX)
  stockInfo$DLRET <- as.factor(stockInfo$DLRET)
  stockInfo$DLRETX <- as.factor(stockInfo$DLRETX)

  #GETDATA - Q2
  datafactors <- read.csv("F-F_Research_Data_Factors.csv",header=TRUE,sep = ",",
                          stringsAsFactors = FALSE,skip = 3,nrows = 1088)
  datafactors$X <- ymd(paste0(datafactors$X,"01"))
  datafactors <- datafactors[datafactors$X >= '1926-07-01' & datafactors$X <= '2015-12-31',]
  datafactors <- datafactors%>%mutate(Year=year(datafactors$X),Month=month(datafactors$X))%>%
                               mutate(Market_Minus_Rf = Mkt.RF,Rf=RF)%>%
                               select(Year,Month,Market_Minus_Rf,SMB,HML,Rf)%>%
                               group_by(Year,Month)%>%
                               summarize(Market_Minus_Rf = prod(1+Market_Minus_Rf/100)-1,
                                         SMB = prod(1+SMB/100)-1,
                                         HML = prod(1+HML/100)-1,
                                         Rf = prod(1+Rf/100)-1)
  
  datafactors <- data.table(datafactors)
  
  save(stockInfo,datafactors,file="Hwk1.RData")
}

#TEST
load("Hwk1.RData")
source("PS1_704873047.R")
monthlydata <- PS1_Q1(stockInfo)
statsresults <- PS1_Q2(monthlydata[Year > 1926 | (Year == 1926 & Month>=7)],datafactors)
corresults <- PS1_Q3(monthlydata,datafactors)

View(stockInfo[month(DATE)==7 & year(DATE)==1962 & PERMNO==10006])
View(stockInfo[month(DATE)==6 & year(DATE)==2004])
View(stockInfo[SHROUT<0])
