from Question1 import generate_uniform_dist
import math
import matplotlib.pyplot as plot
import numpy as np


def question4(n, lambd, seed):
    exp_nums = build_exponential_distribution(n, lambd, seed)

    prob_greater_1 = len([num for num in exp_nums if num >= 1]) / len(exp_nums)
    prob_greater_4 = len([num for num in exp_nums if num >= 4]) / len(exp_nums)

    print("\n QUESTION 4")
    print("Probability that distribution is greater than 1 {0:.4f}".format(prob_greater_1))
    print("Probability that distribution is greater than 4 {0:.4f}".format(prob_greater_4))

    plot.figure()
    plot.hist(exp_nums)
    plot.title("Exponential distribution histogram")
    plot.ylabel("Frequency")
    plot.xlabel("Value")

    print("Mean of exponential distribution = {0:.4f}".format(np.mean(exp_nums)))
    print("Std dev of exponential distribution = {0:.4f}".format(np.std(exp_nums)))


def build_exponential_distribution(n, lambd, seed):
    rand_nums = generate_uniform_dist(n, seed)
    exp_dist = [-lambd * math.log(1 - val) for val in rand_nums]
    return exp_dist

