import statistics
import numpy.random as rand
import numpy as np
import math
import pandas as pd


def question3(seed):
    n = 5000
    rands = rand.standard_normal(n)

    # Part a
    a1 = 5 * (rands**2) + np.sin(rands * math.sqrt(5))
    a2 = math.exp(0.5/2) * np.cos(rands * math.sqrt(0.5))
    a3 = math.exp(3.2 / 2) * np.cos(rands * math.sqrt(3.2))
    a4 = math.exp(6.5 / 2) * np.cos(rands * math.sqrt(6.5))

    answers = np.zeros((4,2))
    answers[0, 0] = statistics.mean(a1)
    answers[1, 0] = statistics.mean(a2)
    answers[2, 0] = statistics.mean(a3)
    answers[3, 0] = statistics.mean(a4)

    answers[0, 1] = statistics.stdev(a1)
    answers[1, 1] = statistics.stdev(a2)
    answers[2, 1] = statistics.stdev(a3)
    answers[3, 1] = statistics.stdev(a4)

    final_ans = pd.DataFrame(answers)
    print(final_ans)

    # Part b
    answers_varRed = np.zeros((4,2))

    a1_control_var = 5 * (rands ** 2)
    var_redn_a1 = control_var_a(a1, a1_control_var, 5)
    answers_varRed[0, 0] = statistics.mean(var_redn_a1)
    answers_varRed[0, 1] = statistics.stdev(var_redn_a1)

    a2_control_var = np.cos(rands * math.sqrt(0.5))
    a2_control_mean = math.exp(-0.5/2.0)
    var_redn_a2 = control_var_a(a2, a2_control_var, a2_control_mean)
    answers_varRed[1, 0] = statistics.mean(var_redn_a2)
    answers_varRed[1, 1] = statistics.stdev(var_redn_a2)

    a3_control_var = np.cos(rands * math.sqrt(3.2))
    a3_control_mean = math.exp(-3.2 / 2.0)
    var_redn_a3 = control_var_a(a3, a3_control_var, a3_control_mean)
    answers_varRed[2, 0] = statistics.mean(var_redn_a3)
    answers_varRed[2, 1] = statistics.stdev(var_redn_a3)

    a4_control_var = np.cos(rands * math.sqrt(6.5))
    a4_control_mean = math.exp(-6.5 / 2.0)
    var_redn_a4 = control_var_a(a4, a4_control_var, a4_control_mean)
    answers_varRed[3, 0] = statistics.mean(var_redn_a4)
    answers_varRed[3, 1] = statistics.stdev(var_redn_a4)

    final_ans_2 = pd.DataFrame(answers_varRed)
    print(final_ans_2)

def control_var_a(a, a_control_var, a_cvar_mean):
    gamma = np.cov(a,a_control_var)[0,1]/statistics.variance(a_control_var)
    #gamma = 1
    final_val = a - gamma * (a_control_var - a_cvar_mean)

    return final_val