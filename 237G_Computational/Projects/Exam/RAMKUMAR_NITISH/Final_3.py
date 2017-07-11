import numpy as np
import numpy.random as rand
import statistics
import matplotlib.pyplot as plot
import scipy.stats as sc
import pandas as pd
import math

def question3():
    np.random.seed(1234)
    s0 = 100
    r = 0.05
    sigma = 0.35
    strike = 100
    time = 5

    no_of_sim = 10000
    dt = 1/100

    stock_path = build_stock_path(s0, r, sigma, time, dt,no_of_sim)

    time_rang = np.array(np.arange(0,time+dt,dt))
    lt = np.transpose(50 * np.exp(0.138629 * time_rang))
    ut = np.transpose(200 - 50 * np.exp(0.138629 * time_rang))

    tu_all = [list(np.where(stock_path[i,:] >= ut)) for i in range(0,no_of_sim)]
    tu = [-1] * no_of_sim
    for tu_ind in range(0,len(tu_all)):
        if (len(tu_all[tu_ind][0]) > 0):
            tu[tu_ind] = np.array(tu_all[tu_ind])[0,0]

    tl_all = [np.array(np.where(stock_path[i,:] <= lt)) for i in range(0,no_of_sim)]
    tl = [-1] * no_of_sim
    for tl_ind in range(0,len(tl_all)):
        if (len(tl_all[tl_ind][0]) > 0):
            tl[tl_ind] = np.array(tl_all[tl_ind])[0,0]

    payoff = [0] * no_of_sim
    itm_count = 0
    tu_win = 0
    tl_win = 0
    for ind in range(0,len(tu)):
        tu_i = tu[ind]
        tl_i = tl[ind]
        if(ind == len(tu)):
            continue

        if (not tu_i == -1 and not tl_i == -1 and tu_i <= tl_i) or (not tu_i == -1 and tl_i == -1) :
            tu_win = tu_win + 1
            itm_count = itm_count + 1
            payoff[ind] = (stock_path[ind,tu_i] - strike) * math.exp(-r * (tu_i/time))
            continue

        if (not tl_i == -1 and not tu_i == -1 and tl_i < tu_i) or (not tl_i == -1 and tu_i == -1):
            tl_win = tl_win + 1
            itm_count = itm_count + 1
            payoff[ind] = (strike - stock_path[ind, tl_i]) * math.exp(-r * (tl_i / time))
            continue

    price = statistics.mean(payoff)
    prob_cond = tl_win/itm_count

    print("Price = {0:.4f}".format(price))
    print("Conditional Probability = {0:.4f}".format(prob_cond))


def build_stock_path(s0, r, sig, T, dt,no_of_sim):
    no_of_steps = int(T/dt)
    rand_nums = np.matrix(rand.standard_normal(no_of_sim*no_of_steps)).reshape(no_of_sim,no_of_steps)
    time = np.arange(1,no_of_steps+1)

    stock_path = np.zeros((no_of_sim,no_of_steps+1))
    stock_path[:,0] = s0
    stock_path[:,1:no_of_steps+1] = s0 * np.exp(np.matrix((r - (sig**2)/2)*np.transpose(time)*dt) +
                                              np.matrix((sig * np.sqrt(dt))*np.cumsum(rand_nums,1)))

    return stock_path