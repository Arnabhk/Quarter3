import numpy as np
from numpy import linalg
from scipy.stats import ncx2
import math
import statistics
import Question1


def question2():
    k = 0.92
    r0 = 0.05
    sigma = 0.12
    ravg = 0.055
    np.random.seed(1234)

    #a
    expiry = 0.5
    strike = 980
    no_of_sim = 250
    time = 1
    step_size = 1/252
    fv = 1000

    #With explicit for underlying
    price_call_dis_bond_2 = price_call_discount_bond(r0, sigma, k, ravg, fv, time, step_size, no_of_sim, expiry, strike)

    #With monte carlo for underlying
    price_call_dis_bond = price_call_discount_bond_mc(r0, sigma, k, ravg, fv, time, step_size, no_of_sim, expiry,strike)
    print("The discount bond call price is {0:.4f}".format(price_call_dis_bond))

    #b
    step_size = 1/365
    dx = 0.001
    price_implicit = calculate_price_implicit(sigma,dx, step_size, r0, time, strike, k, ravg, fv, expiry)
    print("The discount bond call price using implicit method is {0:.4f}".format(price_implicit))

    #c
    explicit_price = price_call_explicit(r0, expiry, time, k, sigma, ravg, strike, fv)
    print("The explicit price of the discount bond call is {0:.4f}".format(explicit_price))

def price_call_discount_bond_mc(r0, sigma, k, ravg, fv, time, step_size, no_of_sim, expiry, strike):
    no_of_steps = int(expiry/step_size)
    call_prices = [None]*no_of_sim
    for simCount in range(1,no_of_sim+1):
        randoms = np.random.normal(0,1,no_of_steps)
        r_path = build_r_path(r0, no_of_steps, k, ravg, step_size, sigma, randoms)
        price = Question1.price_discount_bond(r_path[no_of_steps-1], sigma, k, ravg, fv, time-expiry, 2*(time-expiry)/no_of_steps,
                                    no_of_sim)
        call_prices[simCount-1] = math.exp(-sum(r_path) * step_size)*max(price-strike,0.0)

    return statistics.mean(call_prices)

def price_call_discount_bond(r0, sigma, k, ravg, fv, time, step_size, no_of_sim, expiry, strike):
    no_of_steps = int(expiry/step_size)
    call_prices = [None]*no_of_sim
    for simCount in range(1,no_of_sim+1):
        randoms = np.random.normal(0,1,no_of_steps)
        r_path = build_r_path(r0, no_of_steps, k, ravg, step_size, sigma, randoms)
        price = Question1.price_discount_bond_explicit(r_path[no_of_steps-1], sigma, k, ravg, expiry, time, fv)
        call_prices[simCount-1] = math.exp(-sum(r_path) * step_size)*max(price-strike,0.0)

    return statistics.mean(call_prices)


def calculate_price_implicit(sigma,dx, step_size, r0, time, strike, k, ravg, fv, expiry):

    no_of_paths = int(r0/dx)

    # Generate rates and calculate terminal prices
    rate_path = [r0 + dx*count for count in range(no_of_paths, -no_of_paths - 1, -1)]

    #prices_ter = [max(Question1.price_discount_bond_explicit(rate, sigma, k, ravg, expiry, time, fv) - strike, 0.0)
    #              for rate in rate_path]
    prices_ter = [max(Question1.price_discount_bond(rate, sigma, k, ravg, fv, (time-expiry), 1/252, 1000) - strike, 0.0)
                  for rate in rate_path]

    # Generate call Prices
    max_time_steps = int(expiry / step_size)
    call_prices = np.full([2 * no_of_paths + 1, max_time_steps], np.nan)
    call_prices[:, (max_time_steps - 1)] = prices_ter

    # Calculate price at every time
    for count in range(max_time_steps - 2, -1, -1):
        call_prices[:, count] = calculate_IFD(no_of_paths, call_prices[:, count + 1], rate_path, step_size,
                                              sigma, dx, ravg, k)

    return call_prices[no_of_paths,0]


def calculate_ps(dt, sigma, dx, r, ravg, k):
    pu = -0.5 * dt * (((sigma ** 2) * r / (dx ** 2)) + ((k * (ravg - r)) / dx))
    pm = 1 + dt * ((sigma ** 2)* r / dx ** 2) + r * dt
    pd = -0.5 * dt * (((sigma ** 2) * r / (dx ** 2)) - ((k * (ravg - r)) / dx))

    return [pu, pm, pd]

def build_r_path(r0, no_of_steps, k, ravg, step_size, sigma, randoms):
        r_path = [None] * (no_of_steps + 1)
        r_path[0] = r0

        for count in range(1, no_of_steps + 1):
            dr = k * (ravg - abs(r_path[count - 1])) * step_size + sigma * math.sqrt(step_size) \
                                                                   * math.sqrt(abs(r_path[count - 1])) * randoms[count - 1]
            r_path[count] = abs(r_path[count-1]) + dr
        return r_path

def calculate_IFD(no_of_paths, call_prices_next, r_path, step_size, sigma, dx, ravg, k):
    mat_a = np.zeros([2 * no_of_paths + 1, 2 * no_of_paths + 1])
    mat_a[0, 0] = 1
    mat_a[0, 1] = -1

    for count in range(1, 2 * no_of_paths):
        ps = calculate_ps(step_size, sigma, dx, r_path[count], ravg, k)
        mat_a[count, count - 1] = ps[0]
        mat_a[count, count] = ps[1]
        mat_a[count, count + 1] = ps[2]

    mat_a[2 * no_of_paths, 2 * no_of_paths - 1] = 1
    mat_a[2 * no_of_paths, 2 * no_of_paths] = -1

    mat_b = np.zeros([2 * no_of_paths + 1, 1])
    mat_b[1:(2 * no_of_paths - 1), 0] = call_prices_next[1:(2 * no_of_paths - 1)]
    mat_b[0, 0] = r_path[0] - r_path[1]

    mat_f = np.dot(linalg.inv(mat_a), mat_b)
    return mat_f[:, 0]


def price_call_explicit(r0, expiry, time, k, sigma, ravg, strike, fv):
    ab = calculate_a_b(expiry, time, k, sigma, ravg)

    r_star = math.log(ab[0]/(strike/fv)) / ab[1]
    theta = math.sqrt(k**2 + (2 * (sigma**2)))
    phi = 2*theta/((sigma**2)*(math.exp(theta * expiry)-1))
    psi = (k + theta)/(sigma ** 2)
    price_1 = price_bond_explicit(r0, time, k, sigma, ravg)
    price_2 = price_bond_explicit(r0, expiry, k, sigma, ravg)

    chi2_1 = chisq(2*r_star*(phi+psi+ab[1]),4*k*ravg/(sigma**2),
                   (2 * (phi**2) * r0 * math.exp(theta*expiry))/(phi + psi + ab[1]))

    chi2_2 = chisq(2*r_star*(phi+psi),4*k*ravg/(sigma**2),
                   (2 * (phi**2) * r0 * math.exp(theta*expiry))/(phi + psi))

    call_price = (fv * price_1 * chi2_1) - (strike * price_2 * chi2_2)

    return call_price

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

def chisq(x, p, q):
    return ncx2.cdf(x, p, q)

