import math
import numpy as np
import numpy.random as rand
from itertools import compress
import statistics

def question6(seed):
    a = 0
    b = 1
    h = 1/10000

    ans_a = euler_discretization(h, a, b)
    print("Euler Discretization {0:.4f}".format(ans_a))

    ans_b = importance_sampling(10000,seed)
    print("Importance Sampling {0:.4f}".format(ans_b))

def euler_discretization(h,a,b):
    no_of_steps = int((b-a)/h)
    step_count = np.arange(a+h,b,h)

    x = a + step_count
    vals = [0]*(no_of_steps+1)
    vals[0] = math.sqrt(1 - (a**2))
    vals[1:(no_of_steps-1)] = (h * -x)/np.sqrt(1-(x**2))
    vals = np.cumsum(vals)
    vals[no_of_steps] = math.sqrt(1 - (a**2))

    return sum(vals)*h*4


def importance_sampling(no_of_sim, seed):
    a = 0.74
    uni_rand = rand.uniform(size=no_of_sim)
    tx = (1 - a * (uni_rand**2))/(1-(a/3))
    valid = uni_rand<=tx

    gx = np.sqrt(1-(uni_rand**2))
    ans = (gx/tx)*4.0
    ans_constrained = list(compress(ans,valid))
    return statistics.mean(ans_constrained)


