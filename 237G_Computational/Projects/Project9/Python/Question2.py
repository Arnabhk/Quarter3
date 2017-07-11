import numpy as np
import matplotlib.pyplot as plt
import Question1


def question2():
    wac = 0.08
    loan = 100000
    r0 = 0.078
    k = 0.6
    ravg = 0.08
    sigma = 0.12
    time = 30

    # Part a
    np.random.seed(1234)
    no_of_sim = 1000
    mean_price = Question1.price_mbs(wac, loan, r0, k, ravg, sigma, time, "PSA", no_of_sim)

    print("Mean price of the MBS using the PSA prepayment model is {0:.4f}".format(mean_price))

    # Part b
    k_val = np.arange(0.3, 1, 0.1)
    b_ans = np.zeros([len(k_val), 2])
    no_of_sim = 1000

    for k in range(0, len(k_val)):
        mean_price = Question1.price_mbs(wac, loan, r0, k_val[k], ravg, sigma, time, "PSA",no_of_sim)
        b_ans[k, 0] = k_val[k]
        b_ans[k, 1] = mean_price

    print(b_ans)
    plt.plot(k_val, b_ans[:, 1], 'g--', linewidth=1)
    plt.title("Plot of price with k")
    plt.ylabel("Mortgage price")
    plt.xlabel("k")
    plt.show()


def calculate_cpr(cpr_vals, step):
    if step <= 30:
        return cpr_vals[:,step-1] + 0.002
    else:
        return cpr_vals[:,step-1]