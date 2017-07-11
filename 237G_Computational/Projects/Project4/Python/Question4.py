import numpy as np
import math
import pandas as pd


def question4():
    time = 1;
    k = 100;
    sigma = 0.3;
    r = 0.05;
    delta = time / 12.0;

    s0_all = range(80,124,4)

    answers = [0]*len(s0_all)
    for count in range(0,len(s0_all)):
        answers[count] = amer_price_binomial(r, delta, sigma, 12, s0_all[count], k, "p")
    pd_ans = pd.DataFrame(answers,index=s0_all)
    print(pd_ans)


def amer_price_binomial(r, delta, sigma, noOfSteps, s0, k, optionType):
    d = math.exp((r - (pow(sigma, 2) / 2)) * delta - sigma * math.sqrt(delta));
    u = math.exp((r - (pow(sigma, 2) / 2)) * delta + sigma * math.sqrt(delta));
    prob = 0.5;

    stockPrices = np.zeros((noOfSteps+1,noOfSteps+1))
    optionPrices = np.zeros((noOfSteps + 1, noOfSteps + 1))

    stockPrices[noOfSteps,0] = s0;
    for count in range(1,noOfSteps+1):
        for j in range(1,count+1):
            stockPrices[noOfSteps - j,count] = stockPrices[noOfSteps - j + 1,count - 1] * u;
        stockPrices[noOfSteps,count] = stockPrices[noOfSteps,count - 1] * d;

    #Populate call prices
    for count in range(noOfSteps,-1,-1):
        for j in range(noOfSteps,noOfSteps - count -1,-1):
            putPriceCurrent = max(stockPrices[j][count] - k, 0.0) if optionType == 'c' else \
                              max(k - stockPrices[j][count], 0.0)
            futurePrice = 0.0;
            if not count == noOfSteps:
                futurePrice = (prob * optionPrices[j - 1][count + 1] +
                               (1 - prob) * optionPrices[j][count + 1]) * math.exp(-1.0 * r * delta);
            optionPrices[j,count] = futurePrice if futurePrice >= putPriceCurrent  else putPriceCurrent;

    ret = optionPrices[noOfSteps,0];
    return ret;

