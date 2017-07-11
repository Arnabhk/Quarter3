import matplotlib.pyplot as plot
from Question1 import generate_uniform_dist
from itertools import chain
import numpy as np

def question2(probs, probvals, n, seed):
    finalvals = build_bernoulli_distribution(probs, probvals, n, seed)

    print("\nQUESTION 2")
    print("Mean of Bernoulli distribution is {0:.4f}".format(np.mean(finalvals)))
    print("Std dev of Bernoulli distribution is {0:.4f}".format(np.std(finalvals)))

    ##Histogram
    plot.hist(finalvals)
    plot.title("Plot of Bernoulli Distribution")
    plot.xlabel("Value")
    plot.xticks(probvals)
    plot.ylabel("Frequency")

def build_bernoulli_distribution(probs, probvals, n, seed):
    randnum = generate_uniform_dist(n, seed)
    randnum_np = np.array(randnum)

    startprobs = [round(sum(probs[0:count]),2) for count in range(0, len(probs))]
    endprobs = [round(sum(probs[0:count]),2) for count in range(1, len(probs) + 1)]

    probcounts = [((randnum_np >= start) & (randnum_np < end)).sum() for(start,end) in zip(startprobs,endprobs)]

    bern_dist = [[probval]*probcount for (probcount,probval) in zip(probcounts,probvals)]
    return(list(chain.from_iterable(bern_dist)))