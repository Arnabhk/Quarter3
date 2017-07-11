from Question2 import build_bernoulli_distribution
import matplotlib.pyplot as plot
import math

def question3(n, prob, total, seed):
    bin_dist = build_binomial_dist(n, prob, total, seed)

    print("\nQUESTION 3")
    prob_less_than40 = len([bin for bin in bin_dist if bin >= 40])
    print("Probability that distribution is atleast 40 is {0:.5f}".format(prob_less_than40))

    math_prob = sum([bin_prob(44, count, prob) for count in [40, 41, 42, 43, 44]])
    print("Mathematical probability that distribution is atleast 40 is {0:.5f}".format(math_prob))

    ##Histogram
    plot.figure()
    plot.hist(bin_dist)
    plot.title("Plot of Binomial Distribution")
    plot.xlabel("Value")
    plot.ylabel("Frequency")

def build_binomial_dist(n, prob, total, seed):
    bin_dist = [sum(build_bernoulli_distribution([prob], [1], n, seed * count)) for count in range(1, total + 1)]
    return bin_dist


def bin_prob(n, r, p):
    return nCr(n, r) * (p ** r) * ((1 - p) ** (n - r))


def nCr(n, r):
    f = math.factorial
    return f(n) / f(r) / f(n - r)
