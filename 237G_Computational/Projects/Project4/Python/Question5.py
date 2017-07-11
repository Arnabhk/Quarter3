import numpy as np
import math
import pandas as pds


def question5():
    N = [10, 15, 20, 40, 70, 80, 100, 200, 500]
    time = 0.5;
    r = 0.05;
    sigma = 0.24;
    s0 = 32;
    k = 30;

    answers = np.zeros((len(N),2))

    for count in range(0,len(N)):
        delta = time / N[count];
        #Parta
        d_a = math.exp(-1.0 * sigma * math.sqrt(3 * delta));
        u_a = 1 / d_a;
        pd = (r * delta * (1 - u_a) + pow(r * delta, 2) + pow(sigma, 2) * delta) / ((u_a - d_a) * (1 - d_a));
        pu = (r * delta * (1 - d_a) + pow(r * delta, 2) + pow(sigma, 2) * delta) / ((u_a - d_a) * (u_a - 1));
        pm = 1 - pd - pu;
        answers[count,0] = PriceTrinomial(N[count], r, delta, s0, u_a, d_a, k, pd, pm, pu, False);

        #Part b
        d_b = -1.0 * sigma * math.sqrt(3 * delta);
        u_b = sigma * math.sqrt(3 * delta);
        varnce = (pow(sigma, 2));
        drift = (r - (varnce / 2));
        pd_b = 0.5 * (((varnce * delta + pow(drift, 2) * pow(delta, 2)) / pow(u_b, 2)) - ((drift * delta) / u_b));
        pu_b = 0.5 * (((varnce * delta + pow(drift, 2) * pow(delta, 2)) / pow(u_b, 2)) + ((drift * delta) / u_b));
        pm_b = 1 - pd_b - pu_b;

        answers[count, 1] = PriceTrinomial(N[count], r, delta, math.log(s0), u_b, d_b, k, pd_b, pm_b, pu_b, True);

    ans_pd = pds.DataFrame(answers,index=N,columns=["Normal","LogNormal"])
    print(ans_pd)

def PriceTrinomial(noOfSteps,r, delta, s0, u, d, k, pd, pm, pu, logNormal):
    stockPrices = np.zeros(((2*noOfSteps+1),noOfSteps+1))
    callPrices = np.zeros(((2 * noOfSteps + 1), noOfSteps + 1))

    stockPrices[2 * noOfSteps][0] = s0;
    BuildStockPrice(noOfSteps, stockPrices, u, d, logNormal);
    # Call Prices
    BuildCallPrice(noOfSteps, callPrices, stockPrices, r, delta, k, pu, pm, pd, logNormal);

    callPrice = callPrices[2 * noOfSteps][0];
    return callPrice;

def BuildStockPrice(noOfSteps, stockPrices, u, d, logNormal):
    for count in range(1,noOfSteps+1):
        for j in range(1,(2 * count)):
            stockPrices[(2 * noOfSteps) - j][count] = stockPrices[2 * noOfSteps - j + 1][count - 1];

        stockPrices[(2 * noOfSteps)][count] = stockPrices[2 * noOfSteps][count - 1] + d if logNormal \
            else stockPrices[2 * noOfSteps][count - 1] * d;
        stockPrices[(2 * noOfSteps) - (2 * count)][count] = stockPrices[2 * noOfSteps - (2 * count) + 2][count - 1] + u \
            if logNormal else stockPrices[2 * noOfSteps - (2 * count) + 2][count - 1] * u


def BuildCallPrice(noOfSteps, callPrices, stockPrices, r, delta, k, pu, pm, pd, logNormal):
    for count in range(0,2 * noOfSteps):
        stPrice = math.exp(stockPrices[count][noOfSteps]) if logNormal else stockPrices[count][noOfSteps];
        callPrices[count][noOfSteps] = stPrice - k if stPrice - k >= 0 else 0

    for colcount in range(noOfSteps - 1,-1,-1):
        for j in range(0,(2 * colcount)+1):
            callPrices[2 * noOfSteps - j][colcount] = (pu * callPrices[2 * noOfSteps - j - 2][colcount + 1] +
                                                       pm * callPrices[2 * noOfSteps - j - 1][colcount + 1] +
                                                       pd * callPrices[2 * noOfSteps - j][colcount + 1]) *\
                                                       math.exp(-1.0 * r * delta);
