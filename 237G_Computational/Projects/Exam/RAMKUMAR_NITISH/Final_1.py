import numpy as np
import numpy.random as rand
import statistics
import matplotlib.pyplot as plot
import scipy.stats as sc
import pandas as pd


def question1():
    print("Question 1")
    np.random.seed(1234)

    no_of_sims = 10000
    n = 100

    all_n_vals = np.array(N(1.1,no_of_sims,n))

    ans = np.max(np.column_stack(((4.54 - all_n_vals),np.array([0]*no_of_sims))),axis=1)

    print("Answer = {0:.4f}".format(statistics.mean(ans)))


def N(x, no_of_sims, n):
    # Get a matrix of uniform numbers
    uni_rand = np.random.uniform(size=(no_of_sims, n))

    # Cumulative sum then along the rows
    uni_rand_sum = np.cumsum(uni_rand, axis=1)

    k_pos = [(np.array(np.where(uni_rand_sum[i,:] > x))[0,0] + 1) for i in range(0,no_of_sims)]

    return k_pos