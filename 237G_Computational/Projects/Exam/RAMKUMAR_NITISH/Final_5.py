import numpy as np
import numpy.random as rand
import statistics
import matplotlib.pyplot as plot
import scipy.stats as sc
import pandas as pd
import math

def question5():
    np.random.seed(1234)
    r = 0.05
    q  = 0
    rf = 0.04
    sigma1 = 0.1
    sigma2 = 0.15
    gamma = -0.04
    lamb = 1.5
    strike = 60
    no_of_sim = 10000
    no_of_steps = 100
    s0 = 6000
    e0 = 0.0096
    time = 1
    rho = -0.25
    wt = np.random.standard_normal(size=(no_of_sim,no_of_steps))
    temp = np.random.standard_normal(size=(no_of_sim,no_of_steps))
    bt = rho * wt + math.sqrt(1 - rho**2) * temp

    s_path = build_s_path(r, q, s0,sigma1, gamma, lamb, wt, time, no_of_steps, no_of_sim)
    e_path = build_e_path(r, rf, e0, sigma2, bt, time, no_of_steps, no_of_sim)

    price_1 = s_path[:, no_of_steps] * e_path[:, no_of_steps] - strike
    prices = np.max(np.column_stack((price_1,(np.array([0]*no_of_sim)))),axis=1)
    price = statistics.mean(prices) * math.exp(-r*time)
    print("Price of the option is {0:.4f}".format(price))


def build_s_path(r, q, s0,sigma1, gamma, lamb, wt, time, no_of_steps, no_of_sims):
    step_size = time/no_of_steps
    s_path = np.zeros((no_of_sims, no_of_steps + 1))
    s_path[:, 0] = math.log(s0)
    jumpPos = np.zeros((no_of_sims,no_of_steps))
    tau = generate_exp(1, no_of_sims, lamb)

    for count in range(0,len(tau)):
        if(tau[count] < time - step_size):
            jumpPos[math.ceil(tau[count]/step_size)] = 1

    mu = r - q
    for count in range(1,no_of_steps+1):
        ds = (mu - (sigma1**2)/2)*step_size + (sigma1 * math.sqrt(step_size) * wt[:,(count-1)] + gamma
                                               * jumpPos[:,(count-1)])
        s_path[:,count] = s_path[:,count-1] + ds

    return np.exp(s_path)




def build_e_path(r, rf, e0, sigma2, bt, time, no_of_steps, no_of_sims):
    step_size = time / no_of_steps
    e_path = np.zeros((no_of_sims, no_of_steps + 1))
    e_path[:, 0] = e0

    for count in range(1, no_of_steps + 1):
        de = ((r - rf) * e_path[:,count-1] * step_size) + (sigma2 * bt[:,(count-1)] * np.sqrt(step_size))
        e_path[:,count] = e_path[:,count - 1] + de
    return e_path

def generate_exp(N, no_of_sim, lamb):
    uni_rand = rand.uniform(size=(no_of_sim,N))
    finalVals = -lamb * np.log(1-uni_rand)
    return finalVals

