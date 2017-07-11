import numpy as np
from numpy import linalg
import scipy.stats as st
import pandas as pd
import math


def question1(currPrice):
    sigma = 0.2
    r = 0.04
    dt = 0.002

    time = 0.5;
    strike = 10;

    # Collection of all dx
    dx = [sigma * (math.sqrt(dt)), sigma * (math.sqrt(3 * dt)), sigma * (math.sqrt(4 * dt))]

    # No of Paths based on dx
    no_of_paths = [math.ceil((math.log(currPrice) - math.log(4)) / dx_i) + 20 for dx_i in dx];

    efdanswers = np.zeros([len(range(4, 16 + 1)), len(dx)])
    ifdanswers = np.zeros([len(range(4, 16 + 1)), len(dx)])
    cnfanswers = np.zeros([len(range(4, 16 + 1)), len(dx)])

    # Calculate price at various dx
    for dxcount in range(0, len(dx)):
        efdanswers[:, dxcount] = calculate_price(sigma, r, dx[dxcount], dt, currPrice, no_of_paths[dxcount], time,
                                                 strike, "EFD")
        ifdanswers[:, dxcount] = calculate_price(sigma, r, dx[dxcount], dt, currPrice, no_of_paths[dxcount], time,
                                                 strike, "IFD")
        cnfanswers[:, dxcount] = calculate_price(sigma, r, dx[dxcount], dt, currPrice, no_of_paths[dxcount], time,
                                                 strike, "CNF")

    put_price_bs = [calculate_price_BS(sigma, r, strike, s0, time) for s0 in range(4, 16 + 1)]
    answers = np.column_stack((efdanswers, ifdanswers, cnfanswers, put_price_bs))
    final_answer = pd.DataFrame(answers,
                                columns=['EFD_dt', 'EFD_3dt', 'EFD_4dt', 'IFD_dt', 'IFD_3dt', 'IFD_4dt', 'CNF_dt',
                                         'CNF_3dt', 'CNF_4dt', 'BlackScholes'],
                                index=range(4, 16 + 1))
    return final_answer;


def calculate_price(sigma, r, dx, dt, s0, no_of_paths, time, strike, type):
    ps = calculate_ps(dt, sigma, dx, r, type)
    pu = ps[0]
    pm = ps[1]
    pd = ps[2]

    # Generate Stock Path
    stock_path_log = [math.log(s0) + (count * dx) for count in range(no_of_paths, -no_of_paths - 1, -1)]
    stock_path = [math.exp(st) for st in stock_path_log]
    put_prices_ter = [max(strike - stockVal, 0) for stockVal in stock_path]

    # Generate Put Prices
    max_time_steps = int(time / dt)
    put_prices = np.full([2 * no_of_paths + 1, max_time_steps], np.nan)
    put_prices[:, (max_time_steps - 1)] = put_prices_ter

    # Calculate price at every time
    for count in range(max_time_steps - 2, -1, -1):
        put_prices[:, count] = calculate_put_price(count, no_of_paths, put_prices[:, count + 1], stock_path, pu, pm, pd,
                                                   type)

    # Find the value which is closest to the whole number of stock price
    s0_range = range(4, 16 + 1)
    firstPos = [next(i for i, v in enumerate(stock_path) if v <= count) for count in s0_range]
    putprices_ans = [put_prices[pos - 1, 0] for pos in firstPos]

    return putprices_ans


def calculate_ps(dt, sigma, dx, r, type):
    if type == "EFD":
        pu = dt * (((sigma ** 2) / (2 * (dx ** 2))) + ((r - ((sigma ** 2) / 2)) / (2 * dx)))
        pm = 1 - dt * (sigma ** 2 / dx ** 2) - r * dt
        pd = dt * (((sigma ** 2) / (2 * (dx ** 2))) - ((r - ((sigma ** 2) / 2)) / (2 * dx)))

    elif type == "IFD":
        pu = -0.5 * dt * (((sigma ** 2) / (dx ** 2)) + ((r - ((sigma ** 2) / 2)) / dx))
        pm = 1 + dt * (sigma ** 2 / dx ** 2) + r * dt
        pd = -0.5 * dt * (((sigma ** 2) / (dx ** 2)) - ((r - ((sigma ** 2) / 2)) / dx))

    elif type == "CNF":
        pu = -0.25 * dt * (((sigma ** 2) / (dx ** 2)) + ((r - ((sigma ** 2) / 2)) / dx))
        pm = 1 + dt * (sigma ** 2 / (2 * (dx ** 2))) + (r * dt / 2)
        pd = -0.25 * dt * (((sigma ** 2) / (dx ** 2)) - ((r - ((sigma ** 2) / 2)) / dx))

    return [pu, pm, pd]


def calculate_put_price(time_count, no_of_paths, put_prices_next, stock_path, pu, pm, pd, type):
    if type == "EFD":
        return calculate_EFD(no_of_paths, put_prices_next, stock_path, pu, pm, pd)
    elif type == "IFD":
        return calculate_IFD(no_of_paths, put_prices_next, stock_path, pu, pm, pd)
    elif type == "CNF":
        return calculate_CNF(no_of_paths, put_prices_next, stock_path, pu, pm, pd)


def calculate_EFD(no_of_paths, put_prices_next, stock_path, pu, pm, pd):
    mat_a = np.zeros([2 * no_of_paths + 1, 2 * no_of_paths + 1])
    mat_a[0, 0] = pu
    mat_a[0, 1] = pm
    mat_a[0, 2] = pd
    for count in range(1, 2 * no_of_paths):
        mat_a[count, count - 1] = pu
        mat_a[count, count] = pm
        mat_a[count, count + 1] = pd

    mat_a[2 * no_of_paths, 2 * no_of_paths - 2] = pu
    mat_a[2 * no_of_paths, 2 * no_of_paths - 1] = pm
    mat_a[2 * no_of_paths, 2 * no_of_paths] = pd

    mat_f_next = np.zeros([2 * no_of_paths + 1, 1])
    mat_f_next[:, 0] = put_prices_next

    mat_b = np.zeros([2 * no_of_paths + 1, 1])
    mat_b[2 * no_of_paths, 0] = -(stock_path[2 * no_of_paths] - stock_path[2 * no_of_paths - 1])

    mat_f = np.dot(mat_a, mat_f_next) + mat_b
    return mat_f[:, 0]


def calculate_IFD(no_of_paths, put_prices_next, stock_path, pu, pm, pd):
    mat_a = np.zeros([2 * no_of_paths + 1, 2 * no_of_paths + 1])
    mat_a[0, 0] = 1
    mat_a[0, 1] = -1

    for count in range(1, 2 * no_of_paths):
        mat_a[count, count - 1] = pu
        mat_a[count, count] = pm
        mat_a[count, count + 1] = pd

    mat_a[2 * no_of_paths, 2 * no_of_paths - 1] = -1
    mat_a[2 * no_of_paths, 2 * no_of_paths] = 1

    mat_b = np.zeros([2 * no_of_paths + 1, 1])
    mat_b[1:(2 * no_of_paths - 1), 0] = put_prices_next[1:(2 * no_of_paths - 1)]
    mat_b[2 * no_of_paths, 0] = -(stock_path[2 * no_of_paths] - stock_path[2 * no_of_paths - 1])

    mat_f = np.dot(linalg.inv(mat_a), mat_b)
    return mat_f[:, 0]


def calculate_CNF(no_of_paths, put_prices_next, stock_path, pu, pm, pd):
    mat_a = np.zeros([2 * no_of_paths + 1, 2 * no_of_paths + 1])
    mat_a[0, 0] = 1
    mat_a[0, 1] = -1

    for count in range(1, 2 * no_of_paths):
        mat_a[count, count - 1] = pu
        mat_a[count, count] = pm
        mat_a[count, count + 1] = pd

    mat_a[2 * no_of_paths, 2 * no_of_paths - 1] = -1
    mat_a[2 * no_of_paths, 2 * no_of_paths] = 1

    mat_b = np.zeros([2 * no_of_paths + 1, 1])
    mat_b[1:(2 * no_of_paths - 1), 0] = [-pu * put_prices_next[count - 1] - (pm - 2) * put_prices_next[count]
                                         - pd * put_prices_next[count + 1] for count in range(1, (2 * no_of_paths - 1))]
    mat_b[2 * no_of_paths, 0] = -(stock_path[2 * no_of_paths] - stock_path[2 * no_of_paths - 1])

    mat_f = np.dot(linalg.inv(mat_a), mat_b)
    return mat_f[:, 0]


def calculate_price_BS(sigma, r, strike, s0, time):
    d1 = (math.log(s0 / strike) + (r + (sigma ** 2 / 2)) * time) / (sigma * math.sqrt(time))
    d2 = d1 - sigma * math.sqrt(time)

    put_price = strike * math.exp(-r * time) * st.norm.cdf(-d2) - s0 * st.norm.cdf(-d1)
    return put_price
