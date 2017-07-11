import numpy as np
import numpy.random as rand
import statistics
import matplotlib.pyplot as plot
import scipy.stats as sc
import pandas as pd
import math


def question2():
    np.random.seed(1234)
    v0 = 0.06
    alpha = 0.45
    beta = -5.105
    gamma = 0.25
    no_of_sims = 10000
    no_of_steps = 100
    s0 = 20
    r = 0.05
    rho = [-0.75, 0, 0.75]
    time = 2
    build_paths_calculate(v0,alpha,beta,gamma,no_of_sims,no_of_steps,s0,r,rho, time)


def build_paths_calculate(v0,alpha,beta,gamma,no_of_sims,no_of_steps,s0,r,rhos, time):

    answers = [0] * 3
    step_size = time/no_of_steps

    for rho_count in range(0,len(rhos)):
        rho = rhos[rho_count]
        w_randoms = np.random.standard_normal(size=(no_of_sims,no_of_steps))
        temp_randoms = np.random.standard_normal(size=(no_of_sims,no_of_steps))
        b_randoms = rho * w_randoms + math.sqrt(1 - (rho**2)) * temp_randoms

        v_path = build_v_path(v0, no_of_steps, alpha, beta, gamma, step_size, no_of_sims, w_randoms)
        s_path = build_s_path(s0, no_of_steps, v_path, step_size, no_of_sims, b_randoms, r)

        average_price = np.mean(s_path, axis=1)
        price = np.max(np.column_stack((s_path[:,no_of_steps] - average_price, np.array([0]*no_of_sims))),axis=1)
        answers[rho_count] = statistics.mean(price) * math.exp(-r*time)

    print(answers)

def build_s_path(s0, no_of_steps, vt, step_size, no_of_sims, b_randoms, r):
    s_path = np.zeros((no_of_sims, no_of_steps+1))
    s_path[:,0] = s0

    for count in range(1, no_of_steps + 1):
        v_corrected = np.amax(np.column_stack((vt[:,count - 1], np.array([0.0]*no_of_sims))), axis=1)
        ds = r * s_path[:,count-1] * step_size + (np.sqrt(v_corrected) * b_randoms[:,(count-1)] * np.sqrt(step_size))
        s_path[:,count] = s_path[:,count - 1] + ds
    return s_path


def build_v_path(v0, no_of_steps, alpha, beta, gamma, step_size, no_of_sims, w_randoms):
    v_path = np.zeros((no_of_sims, no_of_steps+1))
    v_path[:,0] = v0

    for count in range(1, no_of_steps + 1):
        v_corrected = np.amax(np.column_stack((v_path[:,count - 1], np.array([0.0]*no_of_sims))), axis=1)
        dv = (alpha + beta * v_corrected) * step_size + (gamma * np.sqrt(v_corrected) *\
                                                        w_randoms[:,(count-1)] * np.sqrt(step_size))
        v_path[:,count] = v_path[:,count - 1] + dv
    return v_path
