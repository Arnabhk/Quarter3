import statistics
import numpy as np
import Question1

def question2(seed):

    mu = [0, 0]
    sd = [1, 1]
    cor = 0.6
    n = 5000
    data = Question1.generate_bivariate_normal(n, seed, mu, sd, cor)
    x = data[:,0]
    y = data[:,1]

    all_data = np.zeros((n,2))
    all_data[:, 0] = x**3 + np.sin(y) + (x**2)*y

    final_data = np.amax(all_data,axis=1)
    return statistics.mean(final_data)