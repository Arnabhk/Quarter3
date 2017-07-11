import statistics
import math
import numpy.random as rand
import numpy as np
import pandas as pd
import scipy.stats as sc

def question4(seed):
    n = 10000
    r = 0.04;
    sig = 0.2;
    s0 = 88;
    T = 5;
    X = 100;

    ans = np.zeros((2,2))

    # Part a
    rand.seed(seed)
    rand_nums = rand.standard_normal(n)
    st_val = generate_stock_path_T(T, s0, r, sig, rand_nums)
    payoff_st = np.amax(np.column_stack((st_val - X,np.array([0]*n))), axis=1)
    ans[0,0] = statistics.mean(payoff_st* math.exp(-r*T))
    ans[0,1] = statistics.stdev(payoff_st * math.exp(-r*T))
    print("Monte carlo price : {0:.4f}".format(ans[0,0]))
    print("BS price : {0:.4f}".format(black_scholes(s0, X, T, r, sig, "C")))

    # Part b
    st_val_anti = generate_stock_path_T(T, s0, r, sig, -rand_nums)
    payoff_st_anti = np.amax(np.column_stack((st_val_anti - X, np.array([0] * n))), axis=1)
    prices = math.exp(-r * T)*((payoff_st + payoff_st_anti)/2)
    ans[1,0] = statistics.mean(prices)
    ans[1,1] = statistics.stdev(prices)
    ans_pd = pd.DataFrame(ans,index=["Normal","Antithetic"],columns=["Mean","Std"])
    print(ans_pd)

def generate_stock_path_T(T, s0, r, sig, rand_nums):
    st = s0 * np.exp((r - (sig**2)/2)*T + (sig * math.sqrt(T) * rand_nums))
    return st


def black_scholes(S0, X, T, r, sigma, type):
    d1 = (math.log(S0 / X) + (r + pow(sigma,2)/2)*T)/(sigma*math.sqrt(T))
    d2 = d1 - sigma*math.sqrt(T)

    test = math.exp(-1.0 * r * T)
    if type == "C":
        price = S0 * sc.norm.cdf(d1) - X * test * sc.norm.cdf(d2)
    else:
        price = X * test * sc.norm.cdf(-d2) - S0 * sc.norm.cdf(-d1)

    return price
