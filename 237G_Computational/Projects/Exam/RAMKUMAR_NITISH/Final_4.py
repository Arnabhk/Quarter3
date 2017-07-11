import numpy as np
import numpy.random as rand
import statistics
import matplotlib.pyplot as plot
import scipy.stats as sc
import pandas as pd
import math

def question4():
    np.random.seed(1234)
    no_of_sim = 10000
    no_of_steps = 100
    time = 0.5
    step_size = time/no_of_steps
    expiry = 1

    r0 = 0.05
    alpha = 0.36
    beta = -5.86
    sigma = 0.36
    gamma = 2
    fv = 10000
    strike = 9800

    price_call_discount_bond_iter(r0, sigma, alpha, beta, gamma, fv, time, step_size, no_of_sim, expiry, strike, no_of_steps)


def price_call_discount_bond_iter(r0, sigma, alpha, beta, gamma, fv, time, step_size, no_of_sim, expiry
                                  , strike, no_of_steps):
    randoms = np.random.normal(0, 1, size=(no_of_sim,no_of_steps))
    r_path = build_r_path(r0, no_of_steps, alpha, beta, gamma, step_size, sigma, no_of_sim, randoms)

    #price = price_discount_bond(r_path[:,no_of_steps], sigma, alpha, beta, gamma, fv, time - expiry,
    #                            (time - expiry) / no_of_steps,
    #                                no_of_sim * 4)
    prices = price_discount_bond_iter(r_path[:,no_of_steps], alpha, beta, gamma, step_size, sigma, fv, (expiry-time), no_of_sim)
    payoff_price = np.max(np.column_stack((strike-prices,np.array([0]*no_of_sim))),axis=1)
    discount = np.exp(-np.sum(r_path,axis=1)*step_size)

    option_price = statistics.mean(payoff_price * discount)
    print("Discount bond put price = {0:.4f}".format(option_price))


#def price_discount_bond(r0, sigma, alpha, beta, gamma, fv, time, step_size, no_of_sim):
#    no_of_steps = int(time/step_size)
#    all_prices = [price_discount_bond_iter(no_of_steps, r0, alpha, beta, gamma, step_size, sigma, fv)
#                  for count in range(0,no_of_sim)]
#    return statistics.mean(all_prices)


def price_discount_bond_iter(r0, alpha, beta, gamma, step_size, sigma, fv, time, no_of_sim):
    no_of_steps = int(time/step_size)
    randoms = np.random.normal(0,1,size=(no_of_sim,no_of_steps))
    r_path = build_r_path(r0, no_of_steps, alpha, beta, gamma, step_size, sigma, no_of_sim, randoms)
    return np.exp(-np.sum(r_path,axis=1)*step_size)*fv



# Function to build path for rates using model
def build_r_path(r0, no_of_steps, alpha, beta, gamma, step_size, sigma, no_of_sims, randoms):
    r_path = np.zeros((no_of_sims, no_of_steps+1))
    r_path[:,0] = r0

    for count in range(1, no_of_steps + 1):
        dr = ((alpha + beta * r_path[:,count-1]) * step_size) + \
             ((sigma * (r_path[:,count-1]**gamma)) * randoms[:,count-1] * math.sqrt(step_size))
        r_path[:,count] = r_path[:,count - 1] + dr
    return r_path
