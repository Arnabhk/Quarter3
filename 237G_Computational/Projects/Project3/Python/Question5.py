import math
import numpy as np
import matplotlib.pyplot as plot
import statistics
import numpy.linalg.cholesky as chol

def question5():
    seq1 = halton_seq(100, 2);
    seq2 = halton_seq(100, 7);

    plot.figure("Figure 1")
    plot.plot(seq1, seq2,"o")
    plot.show()

    seq3 = halton_seq(100, 4)
    plot.figure("Figure 2")
    plot.plot(seq1, seq3, "o")
    plot.show()

    ans_1 = calculate_integral(2, 4, 10000)
    ans_2 = calculate_integral(2, 7, 10000)
    ans_3 = calculate_integral(5, 7, 10000)

    mean_1 = statistics.mean(ans_1)
    mean_2 = statistics.mean(ans_2)
    mean_3 = statistics.mean(ans_3)
    print(mean_1,mean_2,mean_3)


def halton_seq(k,m):
    sequence = [0] * k
    r = math.ceil(math.log(k) / math.log(m)) + 1

    base_rep = [0] * r
    for count in range(0,k):
        j = 0
        done = False
        while not done:
            base_rep[j] = base_rep[j] + 1
            if base_rep[j] < m:
                done = True
            else:
                base_rep[j] = 0
                j = j+1

        for baseCt in range(0,r):
            sequence[count] = sequence[count] + (base_rep[baseCt] / m **(baseCt + 1))

    return sequence


def calculate_integral(m1, m2, k):
    halton_1 = np.array(halton_seq(k, m1))
    halton_2 = np.array(halton_seq(k, m2))

    return np.exp(- halton_1 * halton_2) * (np.sin(6 * math.pi * halton_1) +
                                            ((np.abs(np.cos(2 * math.pi * halton_2))**(1./3.))*
                                            [1 if math.cos(2*math.pi*val)>=0 else -1 for val in halton_2]))
