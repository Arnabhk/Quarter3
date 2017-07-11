import numpy as np
from numpy import linalg
import matplotlib.pyplot as plot


def question2(currPrice):
    sigma = 0.2
    r = 0.04
    dt = 0.002
    time = 0.5;
    strike = 10;

    ds_1 = 0.5
    no_of_paths_1 = int(currPrice * 2 / ds_1)

    ds_2 = 1
    no_of_paths_2 = int(currPrice * 2 / ds_2)

    efdanswers_1 = np.zeros([len(range(4,16+1)),2])
    ifdanswers_1 = np.zeros([len(range(4, 16 + 1)), 2])
    cnfanswers_1 = np.zeros([len(range(4, 16 + 1)), 2])

    efdanswers_1[:,0] = calculate_price(sigma, r, ds_1, dt, no_of_paths_1, time, strike, "EFD", "C")
    ifdanswers_1[:,0] = calculate_price(sigma, r, ds_1, dt, no_of_paths_1, time, strike, "IFD", "C")
    cnfanswers_1[:,0] = calculate_price(sigma, r, ds_1, dt, no_of_paths_1, time, strike, "CNF", "C")

    efdanswers_1[:,1] = calculate_price(sigma, r, ds_1, dt, no_of_paths_1, time, strike, "EFD", "P")
    ifdanswers_1[:,1] = calculate_price(sigma, r, ds_1, dt, no_of_paths_1, time, strike, "IFD", "P")
    cnfanswers_1[:,1] = calculate_price(sigma, r, ds_1, dt, no_of_paths_1, time, strike, "CNF", "P")

    efdanswers_2 = np.zeros([len(range(4,16+1)),2])
    ifdanswers_2 = np.zeros([len(range(4, 16 + 1)), 2])
    cnfanswers_2 = np.zeros([len(range(4, 16 + 1)), 2])

    efdanswers_2[:,0] = calculate_price(sigma, r, ds_2, dt, no_of_paths_2, time, strike, "EFD", "C")
    ifdanswers_2[:,0] = calculate_price(sigma, r, ds_2, dt, no_of_paths_2, time, strike, "IFD", "C")
    cnfanswers_2[:,0] = calculate_price(sigma, r, ds_2, dt, no_of_paths_2, time, strike, "CNF", "C")

    efdanswers_2[:,1] = calculate_price(sigma, r, ds_2, dt, no_of_paths_2, time, strike, "EFD", "P")
    ifdanswers_2[:,1] = calculate_price(sigma, r, ds_2, dt, no_of_paths_2, time, strike, "IFD", "P")
    cnfanswers_2[:,1] = calculate_price(sigma, r, ds_2, dt, no_of_paths_2, time, strike, "CNF", "P")

    answers = np.column_stack((efdanswers_1,ifdanswers_1,cnfanswers_1,efdanswers_2,ifdanswers_2,cnfanswers_2))
    return answers


def calculate_price(sigma, r, ds, dt, no_of_paths, time, strike, type, optiontype):
    # Generate Stock Path
    stock_path = [j * ds for j in range(no_of_paths,-1,-1)]
    option_prices_ter = [max(stockVal - strike, 0) if optiontype == "C" else max(strike - stockVal, 0)
                         for stockVal in stock_path]

    # Generate Call,Put Prices
    max_time_steps = int(time / dt)
    option_prices = np.full([no_of_paths+1, max_time_steps], np.nan)
    option_prices[:, (max_time_steps - 1)] = option_prices_ter

    # Set alpha according to type
    if type == "EFD":
        alpha = 1
    elif type == "IFD":
        alpha = 0
    elif type == "CNF":
        alpha = 0.5

    for count in range(max_time_steps - 2, -1, -1):
        option_prices[:, count] = fd_generalization(alpha, r, sigma, no_of_paths+1, dt,
                                                    optiontype,option_prices[:,count+1], stock_path)
        option_prices[:, count] = [max(option_prices[j,count],option_prices_ter[j]) for j in range(0,no_of_paths+1)]

    s0_range = range(4, 16 + 1)
    first_pos = [next(i for i, v in enumerate(stock_path) if v == count) for count in s0_range]
    option_prices_ans = [option_prices[pos, 0] for pos in first_pos]

    return option_prices_ans


def fd_generalization(alpha, r, sigma, no_of_paths, dt, optiontype, future_price, stock_path):
    a1 = [((sigma ** 2) * (j ** 2) - (r * j)) * (1 - alpha) / 2 for j in range(no_of_paths-1,-1,-1)]
    a2 = [-(1 / dt) - (((sigma ** 2) * (j ** 2) + r) * (1 - alpha)) for j in range(no_of_paths-1,-1,-1)]
    a3 = [((sigma ** 2) * (j ** 2) + (r * j)) * (1 - alpha) / 2 for j in range(no_of_paths-1,-1,-1)]

    b1 = [((sigma ** 2) * (j ** 2) - (r * j)) * alpha / 2 for j in range(no_of_paths-1,-1,-1)]
    b2 = [(1 / dt) - (((sigma ** 2) * (j ** 2) + r) * alpha) for j in range(no_of_paths-1,-1,-1)]
    b3 = [((sigma ** 2) * (j ** 2) + (r * j)) * alpha / 2 for j in range(no_of_paths-1,-1,-1)]

    mat_a = np.zeros([no_of_paths, no_of_paths])
    mat_b = np.zeros([no_of_paths, 1])

    for count in range(1, (no_of_paths-1)):
        mat_a[count, count - 1] = a3[count]
        mat_a[count, count] = a2[count]
        mat_a[count, count + 1] = a1[count]
        mat_b[count, 0] = -b3[count] * future_price[count - 1] - b2[count] * future_price[count] \
                          - b1[count] * future_price[count + 1]

    # set corner cases according to type of option
    mat_a[0, 0] = 1
    mat_a[0, 1] = -1
    mat_a[no_of_paths-1, no_of_paths-2] = 1 if optiontype == "C" else -1
    mat_a[no_of_paths-1, no_of_paths-1] = -1 if optiontype == "C" else 1
    mat_b[0, 0] = (stock_path[0] - stock_path[1]) if optiontype == "C" else 0
    mat_b[no_of_paths - 1, 0] = 0 if optiontype == "C" else \
        -(stock_path[no_of_paths - 1] - stock_path[no_of_paths - 2])

    mat_f = np.dot(linalg.inv(mat_a), mat_b)
    return mat_f[:, 0]


def plotAnswer(answer):
    fig = plot.figure()
    fig.subplots_adjust(hspace=0.5)
    sp1 = fig.add_subplot(211)
    sp1.plot(range(4, 16 + 1), answer[:, 0], 'r--', label="Explicit", linewidth=1, alpha=0.7)
    sp1.set_title("Plot of American Call Prices")
    sp1.set_ylabel("Call prices")
    sp1.set_xlabel("Current stock price")
    sp1.set_yticks(np.arange(min(answer[:, 0]), max(answer[:, 0]) +0.5, 0.50))

    sp1.plot(range(4, 16 + 1), answer[:, 2], 'gs', label="Implicit", linewidth=1, alpha=0.7)
    sp1.plot(range(4, 16 + 1), answer[:, 4], 'b^', label="Crank-Nicolson", linewidth=1, alpha=0.7)
    sp1.legend(loc='upper left')

    sp2 = fig.add_subplot(212)
    sp2.plot(range(4, 16 + 1), answer[:, 1], 'r--', label="Explicit", linewidth=1, alpha=0.7)
    sp2.set_title("Plot of American Put Prices")
    sp2.set_ylabel("Put prices")
    sp2.set_xlabel("Current stock price")
    sp2.set_yticks(np.arange(min(answer[:, 0]), max(answer[:, 0]) + 0.5, 0.50))

    sp2.plot(range(4, 16 + 1), answer[:, 3], 'gs', label="Implicit", linewidth=1, alpha=0.7)
    sp2.plot(range(4, 16 + 1), answer[:, 5], 'b^', label="Crank-Nicolson", linewidth=1, alpha=0.7)
    sp2.legend(loc='upper right')

    fig.show()

