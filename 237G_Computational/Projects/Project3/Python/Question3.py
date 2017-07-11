import math
import numpy as np
import matplotlib.pyplot as plot

def question3(seed, sig, r, s_0, T, X):
    calculate_greeks(X, sig, r, T)


def calculate_greeks(X, r, sig, T):
    s0 = np.array(range(15,26))

    d1 = (np.log(s0/X) + (r + pow(sig, 2) / 2) * T) / (sig* math.sqrt(T))
    d2 = d1 - sig * math.sqrt(T)

    # Delta
    deltas = cdf(d1)
    plot.figure("Figure 1")
    plot.plot(s0,deltas)
    plot.show()

    # Gammas
    gammas = pdf(d1)/(s0 * sig * math.sqrt(T))
    plot.figure("Figure 2")
    plot.plot(s0, gammas)
    plot.show()


    #Theta
    thetas = (-1 * s0 * sig * pdf(d1)/(2*math.sqrt(T))) - (r * X * math.exp(-r*T) * cdf(d2))
    plot.figure("Figure 3")
    plot.plot(s0, thetas)
    plot.show()

    #Vega
    vegas = s0 * math.sqrt(T) * pdf(d1)
    plot.figure("Figure 4")
    plot.plot(s0, vegas)
    plot.show()

    # RHo
    rhos = X * T * math.exp(-r*T) * cdf(d2)
    plot.figure("Figure 5")
    plot.plot(s0, rhos)
    plot.show()

def cdf(x):
    pos = x[np.where(x>=0)]
    neg = -x[np.where(x<0)]

    d1 = 0.0498673470
    d2 = 0.0211410061
    d3 = 0.003277623
    d4 = 0.0000380036
    d5 = 0.0000488906
    d6 = 0.0000053830

    val_pos = 1 + d1 * pos + d2 * (pos**2) + d3 * (pos**3) + d4 * (pos**4) + d5 * (pos**5) + d6 * (pos**6)
    val_neg = 1 + d1 * neg + d2 * (neg ** 2) + d3 * (neg ** 3) + d4 * (neg ** 4) + d5 * (neg ** 5) + d6 * (neg ** 6)

    fin_pos = 1.0 - 0.5 * (val_pos ** (-1.0 * 16))
    fin_neg = 1-(1.0 - 0.5 * (val_neg ** (-1.0 * 16)))

    return np.concatenate((fin_neg,fin_pos),0)


def pdf(x):
    return (1 / math.sqrt(2 * math.pi) * np.exp(-1.0 * (x**2)/2))
