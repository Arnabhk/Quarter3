import numpy as np
import numpy.random as random
import math
import statistics
from scipy.stats import norm

def question3():
    random.seed(1)
    x0 = 0
    y0 = 0
    phi0 = 0.03
    rho = 0.7
    a = 0.1
    b = 0.3
    sigma = 0.03
    n = 0.08

    expiry = 0.5
    time = 1
    fv = 1000
    strike = 950
    step_size = 1/365
    no_of_sim = 500

    put_price = price_put_g2(x0, y0, phi0, rho, a, b, sigma, n, expiry, time, fv, strike, step_size, no_of_sim)
    print("Put price using G2++ model = {0:.4f}".format(put_price))

    put_price_explicit = price_put_explicit(0, time, expiry, x0, y0, a, b, sigma, n, rho, phi0, strike, fv)
    print("Put price explicit answer = {0:.4f}".format(put_price_explicit))

def price_put_g2(x0, y0, phi0, rho, a, b, sigma, n, expiry, time, fv, strike, step_size, no_of_sim):
    no_of_steps = int(expiry/step_size)
    put_prices = [get_put_prices_iterator(no_of_steps, x0, y0, phi0, step_size, sigma, n, a, b, rho, fv, no_of_sim, expiry, time, strike)
      for count in range(0,no_of_sim)]

    return statistics.mean(put_prices)

def get_put_prices_iterator(no_of_steps, x0, y0, phi0, step_size, sigma, n, a, b, rho, fv, no_of_sim, expiry, time, strike):
    x_y_r_path = build_r_path(no_of_steps, x0, y0, phi0, step_size, sigma, n, a, b, rho)
    price = price_discount_bond(expiry, time, x_y_r_path[no_of_steps - 1, 0], x_y_r_path[no_of_steps - 1, 1],
                                phi0, sigma, n, a, b, rho, no_of_sim, step_size, fv)

    ter_price = max(strike - price, 0.0)
    return math.exp(-sum(x_y_r_path[:, 2]) * step_size) * ter_price

def build_r_path(no_of_steps, x0, y0, phi0, step_size, sigma, n, a, b, rho):
    x = [None] * (no_of_steps + 1)
    y = [None] * (no_of_steps + 1)
    r_path = [None] * (no_of_steps + 1)
    x[0] = x0
    y[0] = y0
    r_path[0] = x[0] + y[0] + phi0

    rand1 = np.random.normal(0, 1, no_of_steps)
    rand2 = np.random.normal(0, 1, no_of_steps)
    for count in range(1, no_of_steps + 1):
        x[count] = x[count - 1] - (a * x[count - 1] * step_size) + sigma * math.sqrt(step_size) * rand1[count - 1]
        y[count] = y[count - 1] - (b * y[count - 1] * step_size) + n * math.sqrt(step_size) \
                                                                   * (
                                                                   rho * rand1[count - 1] + math.sqrt(1 - (rho ** 2)) *
                                                                   rand2[count - 1])
        r_path[count] = x[count] + y[count] + phi0

    return np.column_stack((x,y,r_path))

def price_discount_bond(t0, time, x0, y0, phi0, sigma, n, a, b, rho, no_of_sim, step_size, fv):
    no_of_steps = int((time - t0)/step_size)
    prices = [get_bond_prices_iterator(no_of_steps, x0, y0, phi0, step_size, sigma, n, a, b, rho) for count in range(0,no_of_sim)]

    return statistics.mean(prices)*fv


def get_bond_prices_iterator(no_of_steps, x0, y0, phi0, step_size, sigma, n, a, b, rho):
    path = build_r_path(no_of_steps, x0, y0, phi0, step_size, sigma, n, a, b, rho)
    return math.exp(-1.0 * sum(path[:,2]) * step_size)


def price_discount_bond_explicit(t0, Time, xt0, yt0, a, b, sigma, n, rho, phi0):
    v = ((sigma**2)/(a**2)) * (Time - t0 + (2/a)*math.exp(-a*(Time-t0)) - (1/(2*a))*math.exp(-2*a * (Time-t0)) -(3/(2*a)))+\
        ((n**2)/(b**2)) * (Time - t0 + (2/b)*math.exp(-b*(Time-t0)) - (1/(2*b))*math.exp(-2*b * (Time-t0))-(3/(2*b)))+\
        2*rho*(sigma*n/(a*b))*(Time - t0 + ((math.exp(-a*(Time-t0)) - 1)/a) + ((math.exp(-b*(Time-t0)) - 1)/b) -
                               (math.exp(-((a+b)*(Time-t0))) - 1)/(a+b))

    price = math.exp(((t0-Time)*phi0) + ((math.exp(-a*(Time-t0) - 1))*xt0/a) + ((math.exp(-b*(Time-t0) - 1))*yt0/b) +\
            0.5*v)

    return price


def price_put_explicit(t0, Time, expiry, xt0, yt0, a, b, sigma, n, rho, phi0, strike, fv):
    sigmasq_1 = (((sigma**2)/(2*(a**3)))*((1 - math.exp(-a * (Time-expiry)))**2) * (1 - math.exp(-2*a*(expiry-t0))))
    sigmasq_2 = (((n**2)/(2*(b**3)))*((1 - math.exp(-b * (Time-expiry)))**2) * (1 - math.exp(-2*b*(expiry-t0))))
    sigmasq_3 = (2*rho*(sigma*n/(a*b*(a+b)))*(1-math.exp(-a*(Time-expiry)))*(1-math.exp(-b*(Time-expiry)))*(1-math.exp(-(a+b)*(expiry-t0))))

    sigmasq = sigmasq_1 + sigmasq_2 + sigmasq_3

    s_bond = price_discount_bond_explicit(0, Time, xt0, yt0, a, b, sigma, n, rho, phi0)
    t_bond = price_discount_bond_explicit(0, expiry, xt0, yt0, a, b, sigma, n, rho, phi0)

    put_price = -fv * s_bond * norm.cdf(((math.log((strike * t_bond)/(fv * s_bond)))/(math.sqrt(sigmasq))) - 0.5 * math.sqrt(sigmasq)) +\
                t_bond * strike * norm.cdf(((math.log((strike * t_bond)/(fv * s_bond)))/(math.sqrt(sigmasq))) + 0.5 * math.sqrt(sigmasq))

    return put_price