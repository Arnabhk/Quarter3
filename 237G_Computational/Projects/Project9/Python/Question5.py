import Question1
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def question5(oas):
    y = 0.0005
    wac = 0.08
    loan = 100000
    r0 = 0.078
    k = 0.6
    sigma = 0.12
    time = 30
    no_of_sim = 1000

    # Part a
    ravg_list = np.arange(0.03,0.1,0.01)

    answers = np.zeros([len(ravg_list),3])

    for ravg_c in range(0,len(ravg_list)):

        ravg = ravg_list[ravg_c]
        io_price = Question1.price_mbs(wac, loan, r0, k, ravg, sigma, time, "NUMERIX", no_of_sim, oas, "IO")

        po_price = Question1.price_mbs(wac, loan, r0, k, ravg, sigma, time, "NUMERIX", no_of_sim, oas, "PO")

        answers[ravg_c, 0] = ravg
        answers[ravg_c, 1] = io_price
        answers[ravg_c, 2] = po_price

    answers_pd = pd.DataFrame(answers,index=ravg_list,columns=["RAvg","IO","PO"])
    print(answers_pd)

    plt.plot(answers[:,0], answers[:,1], 'g--', linewidth=1, label="IO")
    plt.title("Plot of IO with ravg")
    plt.ylabel("price")
    plt.xlabel("ravg")
    plt.plot(answers[:, 0], answers[:, 2], 'r--', linewidth=1,label="PO")
    plt.legend(loc='upper right')

    plt.show()




