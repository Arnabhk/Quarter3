import statistics
from numpy import random

def question1(n, seed):
    randnum = generate_uniform_dist(n, seed)
    mean = statistics.mean(randnum)
    stddev = statistics.stdev(randnum)
    print("Mean of LGM is {0:.4f}".format(mean))
    print("Standard Deviation of LGM is {0:.4f}".format(stddev))

    inbuilt_random = random.uniform(0,1,n)
    mean_inbuilt = statistics.mean(inbuilt_random)
    stddev_inbuilt = statistics.mean(inbuilt_random)
    print("Mean of Inbuilt is {0:.4f}".format(mean_inbuilt))
    print("Standard Deviation of Inbuilt is {0:.4f}".format(stddev_inbuilt))


def generate_uniform_dist(N, seed):
    m = 2 ** 31 - 1
    a = 7 ** 5
    b = 0

    randnum = [seed]

    for count in range(1, N+1):
        temp = a * (randnum[count - 1]) + b
        randnum.append(temp % m)

    del randnum[0]
    randnum = [(val + 0.5) / m for val in randnum]

    return randnum


def generate_uniform_dist_prev(N, prev_val):
    m = 2 ** 31 - 1
    seed = prev_val * m - 0.5
    return generate_uniform_dist(N,seed)

