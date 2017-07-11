import numpy as np
import numpy.random as rand
import math
import statistics


def question1(seed):
    n = 1000
    mu = [0, 0]
    sd = [1, 1]
    cor = -0.7

    data = generate_bivariate_normal(n, seed, mu, sd, cor)

    x1 = data[:,0]
    x2 = data[:,1]

    nmtr = sum((x1 - statistics.mean(x1)) * (x2 - statistics.mean(x2)))
    dmtr = math.sqrt(sum((x1 - statistics.mean(x1))**2)) * math.sqrt(sum((x2 - statistics.mean(x2))**2))
    return nmtr/dmtr


def generate_bivariate_normal(n, seed , mu, sd, cor):
    rand.seed(seed)
    z = rand.standard_normal(2*n)

    z1 = z[0:n]
    z2 = z[n:]

    x1 = mu[0] + z1 * sd[0]
    a = sd[0] * cor
    b = sd[1] * math.sqrt(1 - (cor**2))
    x2 = mu[1] + z1 * a + z2 * b

    return np.column_stack((x1,x2))