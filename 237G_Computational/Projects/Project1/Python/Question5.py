from time import time
from Question1 import generate_uniform_dist
from Question1 import generate_uniform_dist_prev
from math import *
import numpy as np


def question5(seed):
    n = 5000
    means, sds, time_taken = normal_dist_stats(n, seed)
    print("Mean of Box Muller is = {0}\n".format(means[0]))
    print("Mean of Polar Magsarlia is = {0}\n".format(means[1]))

    print("Std of Box Muller is = {0}\n".format(sds[0]))
    print("Std of Polar Magsarlia is = {0}\n".format(sds[1]))

    print("Time taken for Box Muller is = {0}\n".format(time_taken[0]))
    print("Time taken for Polar Magsarlia is = {0}\n".format(time_taken[1]))


def normal_dist_stats(n, seed):
    start_time_1 = time()
    normal_box_muller = generate_normal_box_buller(n, seed)
    end_time_1 = time()
    bm_mean = np.mean(normal_box_muller)
    bm_sd = np.std(normal_box_muller)

    start_time_2 = time()
    normal_polar_marsaglia = generate_normal_pol_glia(n, seed)
    end_time_2 = time()
    pm_mean = np.mean(normal_polar_marsaglia)
    pm_sd = np.std(normal_polar_marsaglia)

    return [bm_mean, pm_mean], [bm_sd, pm_sd],[end_time_1-start_time_1,end_time_2-start_time_2]

def generate_normal_box_buller(n, seed):
    rand_num = generate_uniform_dist(n, seed)
    rand_num_1 = rand_num[0 : int(len(rand_num) / 2)]
    rand_num_2 = rand_num[int(len(rand_num) / 2): len(rand_num)]

    z1 = [sqrt(-2 * log(val1)) * cos(2 * pi * val2) for (val1, val2) in zip(rand_num_1, rand_num_2)]
    z2 = [sqrt(-2 * log(val1)) * sin(2 * pi * val2) for (val1, val2) in zip(rand_num_1, rand_num_2)]

    normal_dist = z1 + z2
    return normal_dist


def generate_normal_pol_glia(n, seed):
    normal_dist = []

    count = 0
    rand_nums = [seed]
    while count < (n / 2):
        rand_nums = generate_uniform_dist_prev(2, rand_nums[len(rand_nums)-1])
        v1 = 2 * rand_nums[0] - 1
        v2 = 2 * rand_nums[1] - 1
        w = (v1 ** 2) + (v2 ** 2)
        if w > 1:
            continue
        count = count + 1
        z1 = v1 * sqrt(-2 * log(w) / w);
        z2 = v2 * sqrt(-2 * log(w) / w);
        normal_dist.append(z1)
        normal_dist.append(z2)

    return normal_dist
