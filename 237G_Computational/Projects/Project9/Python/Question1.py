import math
import numpy as np
from operator import mul
import statistics
import matplotlib.pyplot as plt
import Question2


def question1():
    wac = 0.08
    loan = 100000
    r0 = 0.078
    k = 0.6
    ravg = 0.08
    sigma = 0.12
    time = 30

    # Part a
    no_of_sim = 1000
    np.random.seed(1234)

    mean_price = price_mbs(wac, loan, r0, k, ravg, sigma, time,"NUMERIX",no_of_sim)

    print("Mean price of the MBS using the Numerix prepayment model is {0:.4f}".format(mean_price))

    # Part b
    k_val = np.arange(0.3, 0.9, 0.1)
    b_ans = np.zeros([len(k_val),2])
    no_of_sim = 1000
    for k in range(0, len(k_val)):
        mean_price = price_mbs(wac, loan, r0, k_val[k], ravg, sigma, time,"NUMERIX",no_of_sim)
        b_ans[k,0] = k_val[k]
        b_ans[k,1] = mean_price

    print(b_ans)

    plt.plot(k_val, b_ans[:, 1], 'r--', linewidth=1)
    plt.title("Plot of price with k")
    plt.ylabel("Mortgage price")
    plt.xlabel("k")
    plt.show()

    # Part c
    k = 0.6
    ravg_val = np.arange(0.03, 0.1, 0.01)
    c_ans = np.zeros([len(ravg_val), 2])

    for rval in range(0, len(ravg_val)):
        mean_price = price_mbs(wac, loan, r0, k, ravg_val[rval], sigma, time, "NUMERIX",no_of_sim)
        c_ans[rval, 0] = ravg_val[rval]
        c_ans[rval, 1] = mean_price

    print(c_ans)

    plt.plot(ravg_val, c_ans[:, 1], 'g--', linewidth=1)
    plt.title("Plot of price with r_avg")
    plt.ylabel("Mortgage price")
    plt.xlabel("Ravg")
    plt.show()

def price_mbs(wac, loan, r0, k, ravg, sigma, time, type, no_of_sims, x=0,io_po="None"):
    np.random.seed(9999)
    step_size = 1/12
    r = wac/12

    no_of_steps_r = int((time+10) / step_size)
    randoms = np.random.normal(0, 1, size=(no_of_sims, no_of_steps_r))
    # Build path for rates using CIR model
    r_path = build_r_path(r0, no_of_steps_r, k, ravg, step_size, sigma, no_of_sims, randoms)
    # We need 10 years rate from year 30 to calculate r(10) at time 30
    r10_path = construct_r10(r_path, step_size, time)

    no_of_steps_loan = int(time/step_size)
    pv = np.zeros((no_of_sims,no_of_steps_loan+1))
    cpr_vals = np.zeros((no_of_sims, no_of_steps_loan+1))
    ip = np.zeros((no_of_sims, no_of_steps_loan + 1))
    tpp = np.zeros((no_of_sims, no_of_steps_loan + 1))
    cpr_vals[:,0] = 0
    pv[:,0] = loan
    ip[:,0] = 0
    tpp[:,0] = 0
    price = [0.0] * no_of_sims

    for step in range(1,(no_of_steps_loan+1)):
        ct = calculate_ct(r, no_of_steps_loan, pv, step, r10_path, cpr_vals,type)
        ip[:,step] = pv[:,step-1]*r
        tpp[:,step] = ct - ip[:,step]
        pv[:,step] = pv[:,step-1] - tpp[:,step]

        #discount_price = price_bond_explicit(r0, step*step_size, k, sigma, ravg)
        discount_price = price_bond(r_path, step, step_size)

        if io_po == "None":
            price = price + ct * discount_price * math.exp(-step*step_size*x)
        elif io_po == "IO":
            price = price + ip[:,step] * discount_price * math.exp(-step*step_size*x)
        elif io_po == "PO":
            price = price + tpp[:,step] * discount_price * math.exp(-step*step_size*x)

    return statistics.mean(price)


# Function to build path for rates using CIR model
def build_r_path(r0, no_of_steps, k, ravg, step_size, sigma, no_of_sims, randoms):
    r_path = np.zeros((no_of_sims, no_of_steps+1))
    r_path[:,0] = r0

    w = sigma * math.sqrt(step_size) * randoms
    for count in range(1, no_of_steps + 1):
        r_corrected = np.amax(np.column_stack((r_path[:,count - 1], np.array([0.0]*no_of_sims))), axis=1)
        dr = k * (ravg - r_corrected) * step_size + (np.sqrt(r_corrected) * w[:,(count-1)])
        r_path[:,count] = r_path[:,count - 1] + dr
    return r_path


def construct_r10(r_path, step_size, time):
    # r10 is average of rates from a given time
    steps_r10 = int(10/step_size)
    r10 = [np.sum(r_path[:,(count+1):(count+steps_r10+1)], axis=1)/120 for count in range(0,(time*12))]
    return np.transpose(np.asarray(r10))


def calculate_ct(r, no_of_steps, pv, step, r10_path, cpr_vals, type):

    # Construct r(10) using CIR model
    if type == "NUMERIX":
        cpr = calculate_cpr(r, r10_path, pv, step)
    elif type=="PSA":
        cpr = Question2.calculate_cpr(cpr_vals, step)

    cpr_vals[:,step] = cpr
    ct_1 = pv[:,step-1] * r / (1 - (1+r)**(-no_of_steps + step-1))
    ct_2 = (pv[:,step-1] - (pv[:,step-1] * r * ((1/(1 - (1+r)**(-no_of_steps + step-1)))-1))) * (1-((1-cpr)**(1/12)))
    ct = ct_1 + ct_2

    return ct


def calculate_cpr(mort_rate, r10_path, pv, step):
    ri = 0.28 + 0.14 * np.arctan(-8.57 + 430 * (12 * (mort_rate) - r10_path[:,step-1]))
    bu = 0.3 + 0.7 * (pv[:,step-1]/pv[:,0])
    sg = min(1.0, step/30)

    sy_const = [0.94, 0.76, 0.74, 0.95, 0.98, 0.92, 0.98, 1.1, 1.18, 1.22, 1.23, 0.98]
    sy_indicator = [1 if ((step-1)%12 == count) else 0 for count in range(0,11)]
    sy = sum(map(mul, sy_const, sy_indicator))

    cpr = ri * bu * sg * sy
    return cpr


def price_bond(r_path, step, step_size):
    return statistics.mean(np.exp(-np.sum(r_path[:,0:step],axis=1)* step_size))


def price_bond_explicit(rt, time, k, sigma, ravg):
    ab = calculate_a_b(0, time, k, sigma, ravg)
    return ab[0] * math.exp(-ab[1]*rt)


def calculate_a_b(expiry, time, k , sigma, ravg):
    h1 = math.sqrt(k ** 2 + 2 * (sigma ** 2))
    h2 = (k + h1) / 2
    h3 = (2 * k * ravg) / (sigma ** 2)

    nmtr_b = math.exp(h1 * (time-expiry)) - 1
    dmtr_b = h2 * (math.exp(h1 * (time-expiry)) - 1) + h1

    b = nmtr_b/dmtr_b

    nmtr_a = h1 * math.exp(h2 * (time-expiry))
    dmtr_a = h2 * (math.exp(h1 * (time-expiry)) - 1) + h1

    a = (nmtr_a/dmtr_a) ** h3

    return [a,b]

