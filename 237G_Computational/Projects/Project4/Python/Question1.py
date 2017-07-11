import math
import numpy as np
import pandas as pd


def question1():
    sigma = 0.24;
    r = 0.05;
    s0 = 32;
    K = 30;
    time = 0.5;

    no_of_steps = [10, 20, 40, 80, 100, 200, 500]
    answers = np.zeros((4,len(no_of_steps)))


    for count in range(0,len(no_of_steps)):
        delta = time / no_of_steps[count];
        answers[0,count] = Parta(r, delta, sigma, no_of_steps[count], s0, K)
        answers[1,count] = Partb(r, delta, sigma, no_of_steps[count], s0, K)
        answers[2,count] = Partc(r, delta, sigma, no_of_steps[count], s0, K, "c")
        answers[3,count] = Partd(r, delta, sigma, no_of_steps[count], s0, K)

    ans_pd = pd.DataFrame(answers,index=["Method1","Method2","Method3","Method4"],columns=no_of_steps)
    print(ans_pd)

def Parta(r, delta, sigma, no_of_steps, s0, K):
    c = (math.exp(-1.0 * r * delta) + math.exp((r + pow(sigma, 2))*delta)) / 2.0;
    d = c - math.sqrt(pow(c, 2) - 1);
    u = 1.0 / d;
    prob = (math.exp(r*delta) - d) / (u - d);
    return PriceOption(r, no_of_steps, delta, prob, u, d, s0, K)


def Partb(r, delta, sigma, no_of_steps, s0, K):
    d = math.exp(r*delta)*(1 - math.sqrt(math.exp(pow(sigma, 2)*delta) - 1));
    u = math.exp(r*delta)*(1 + math.sqrt(math.exp(pow(sigma, 2)*delta) - 1));
    prob = 0.5;
    return PriceOption(r, no_of_steps, delta, prob, u, d, s0, K);


def Partc(r, delta, sigma, noOfSteps, s0, K, optionType="c"):
    d = math.exp((r - (pow(sigma, 2) / 2))*delta - sigma*math.sqrt(delta));
    u = math.exp((r - (pow(sigma, 2) / 2))*delta + sigma*math.sqrt(delta));
    prob = 0.5;
    return PriceOption(r, noOfSteps, delta, prob, u, d, s0, K, optionType);


def Partd(r, delta, sigma, no_of_steps, s0, K):
    d = math.exp(-1.0 * sigma*math.sqrt(delta))
    u = math.exp(sigma*math.sqrt(delta))
    prob = 0.5 + 0.5 * ((r - (pow(sigma, 2) / 2))*math.sqrt(delta)) / sigma
    return PriceOption(r, no_of_steps, delta, prob, u, d, s0, K)


def PriceOption(r, no_of_steps, delta, prob, u, d, s0, K, option_type = "c"):
    time = np.array(range(0,(no_of_steps + 1)))

    st = s0 * np.power(u,time) * np.power(d,(no_of_steps-time))
    price = np.amax(np.column_stack((st-K,np.array([0]*(no_of_steps+1)))),axis=1) if option_type == "c" else\
            np.amax(np.column_stack((K-st,np.array([0]*(no_of_steps+1)))),axis=1)

    bnl_probs = [bnl_prob(no_of_steps, t, prob) for t in time]

    return sum(price * bnl_probs) * math.exp(- r * no_of_steps * delta)


def bnl_prob(n, k, prob):
    return nCr(n,k) * (prob**k) * ((1-prob)**(n-k))
    #return math.exp(sc.gamma(n + 1) - sc.gamma(k + 1) - sc.gamma(n - k + 1))* pow(prob, k)*pow(1 - prob, n - k)


def nCr(n,r):
    f = math.factorial
    return f(n) // f(r) // f(n-r)