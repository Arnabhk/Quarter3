import numpy as np
import scipy.stats as st
import math
import statistics
import scipy.optimize as opt

def question1():
    r0 = 0.05
    sigma = 0.1
    k = 0.82
    ravg = 0.05
    no_of_sim = 10000

    #a
    fv = 1000
    time = 0.5
    step_size = 1/252
    np.random.seed(1234)
    price_dis_bond = price_discount_bond(r0, sigma, k, ravg, fv, time, step_size, no_of_sim)
    print("The pure discount bond is {0:.4f}".format(price_dis_bond))

    #b
    time = 4
    coupon = 30
    no_of_sim = 500
    price_coup_bond = price_coupon_bond(r0, sigma, k, ravg, fv, time, step_size, no_of_sim, coupon)
    print("The coupon paying bond is {0:.4f}".format(price_coup_bond))

    #c
    expiry = 0.25
    strike = 980
    no_of_sim = 10000
    time = 0.5
    price_call_dis_bond = price_call_discount_bond(r0, sigma, k, ravg, fv, time, step_size, no_of_sim, expiry, strike)
    #price_call_dis_bond_2 = price_call_discount_bond_mc(r0, sigma, k, ravg, fv, time, step_size, no_of_sim, expiry, strike)
    print("The discount bond call price is {0:.4f}".format(price_call_dis_bond))

    #d
    no_of_sim = 250
    time = 4
    call_coupon_mc = price_call_coupon_mc(r0, sigma, k, ravg, fv, time, step_size, no_of_sim, expiry, strike, coupon)
    print("The price of the call on coupon bond is {0:.4f}".format(call_coupon_mc))

    #e
    call_coupon_explicit = price_call_coupon_exp(r0, sigma, k, ravg, fv, time, expiry, strike, coupon)
    print("The price of the call on coupon bond through explicit method is {0:.4f}".format(call_coupon_explicit))


#Helps in building rate path
def build_r_path(r0, no_of_steps, k, ravg, step_size, sigma, randoms):
    r_path = [None] * (no_of_steps + 1)
    r_path[0] = r0

    for count in range(1, no_of_steps + 1):
        r_path[count] = r_path[count - 1] + k * (ravg - r_path[count - 1]) * step_size + sigma * math.sqrt(step_size) \
                                                                                         * randoms[count - 1]
    return r_path


#Helps in pricing a pure discount bond
def price_discount_bond(r0, sigma, k, ravg, fv, time, step_size, no_of_sim):
    no_of_steps = int(time/step_size)
    all_prices = [price_discount_bond_iter(no_of_steps, r0, k, ravg, step_size, sigma, fv) for count in range(0,no_of_sim)]
    return statistics.mean(all_prices)


def price_discount_bond_iter(no_of_steps, r0, k, ravg, step_size, sigma, fv):
    randoms = np.random.normal(0,1,no_of_steps)
    r_path = build_r_path(r0, no_of_steps, k, ravg, step_size, sigma, randoms)
    return math.exp(-sum(r_path)*step_size)*fv


# Helps in calculating explicit price of pure discount bond
def price_discount_bond_explicit(rt, sigma, k, ravg, time, T, fv):
    b = (1-math.exp(-k*(T-time)))/k
    a = math.exp((ravg - (sigma**2/(2*(k**2))))*(b - (T-time)) - ((sigma**2)/(4*k))*(b**2))

    return fv * a * math.exp(-b*rt)


#Helps price coupon bond
def price_coupon_bond(r0, sigma, k, ravg, fv, time, step_size, no_of_sim, coupon):
    cashflows = [ fv+coupon if (i == time) else coupon for i in np.arange(0.5,time+0.5,0.5)]
    times = np.arange(0.5,time+0.5,0.5)
    prices = [price_discount_bond(r0, sigma, k, ravg, cashflows[cfcount],times[cfcount],step_size,no_of_sim)
              for cfcount in range(0,len(cashflows))]
    total_price = sum(prices)

    return total_price


# Helps price call on pure discount bond (using explicit for underlying)
def price_call_discount_bond(r0, sigma, k, ravg, fv, time, step_size, no_of_sim, expiry, strike):
    no_of_steps = int(expiry/step_size)

    call_prices = [price_call_discount_bond_iter(r0, sigma, k, ravg, fv, time, step_size, no_of_sim, expiry,
                                                 strike, no_of_steps, True) for count in range(0, no_of_sim)]
    return statistics.mean(call_prices)

#Helps price call on pure discount bond (using Monte carlo for underlying)
def price_call_discount_bond_mc(r0, sigma, k, ravg, fv, time, step_size, no_of_sim, expiry, strike):
    no_of_steps = int(expiry/step_size)

    call_prices = [price_call_discount_bond_iter(r0, sigma, k, ravg, fv, time, step_size, no_of_sim, expiry,
                                                 strike, no_of_steps, False) for count in range(0,no_of_sim)]
    return statistics.mean(call_prices)


def price_call_discount_bond_iter(r0, sigma, k, ravg, fv, time, step_size, no_of_sim, expiry, strike, no_of_steps, is_explicit):
    randoms = np.random.normal(0, 1, no_of_steps)
    r_path = build_r_path(r0, no_of_steps, k, ravg, step_size, sigma, randoms)

    if not is_explicit:
        price = price_discount_bond(r_path[no_of_steps - 1], sigma, k, ravg, fv, time - expiry,
                                    2 * (time - expiry) / no_of_steps,
                                    no_of_sim * 4)
    else:
        price = price_discount_bond_explicit(r_path[no_of_steps - 1], sigma, k, ravg, expiry, time, fv)

    return math.exp(-sum(r_path) * step_size) * max(price - strike, 0.0)


# helps price call option on coupon paying bond
def price_call_coupon_mc(r0, sigma, k, ravg, fv, time, step_size, no_of_sim, expiry, strike, coupon):
    cashflows = [fv + coupon if (i == time) else coupon for i in np.arange(0.5, time + 0.5, 0.5)]
    times = np.arange(0.5, time + 0.5, 0.5)
    r_star = opt.newton(calculate_rstar,ravg,args=(cashflows, times, k, sigma, ravg, expiry, strike))

    call_coupon_price = 0.0
    for cfcount in range(0,len(cashflows)):
        strike_star = price_discount_bond_explicit(r_star, sigma, k, ravg, expiry, times[cfcount], 1)
        call_coupon_price += cashflows[cfcount] * price_call_discount_bond_mc(r0, sigma, k, ravg, 1, time, step_size,
                                                         int(no_of_sim/5), expiry, strike_star)
    return call_coupon_price


def price_call_coupon_iter(r_star, sigma, k, ravg, expiry, cf, cftime, time , r0, step_size, no_of_sim):
        strike_star = price_discount_bond_explicit(r_star, sigma, k, ravg, expiry, cftime, 1)
        return cf * price_call_discount_bond_mc(r0, sigma, k, ravg, 1, time, step_size,
                                                         int(no_of_sim/5), expiry, strike_star)


#Helps calculate price of call on coupon bond explicitly
def price_call_coupon_exp(r0, sigma, k, ravg, fv, time, expiry, strike, coupon):
    cashflows = [fv + coupon if (i == time) else coupon for i in np.arange(0.5, time + 0.5, 0.5)]
    times = np.arange(0.5, time + 0.5, 0.5)
    r_star = opt.newton(calculate_rstar, ravg, args=(cashflows, times, k, sigma, ravg, expiry, strike))

    coupon_prices = [calculate_exp_price_iter(r_star, sigma, k, ravg, expiry, times[count], r0, cashflows[count])
                    for count in range(0, len(cashflows))]

    return sum(coupon_prices)

def calculate_rstar(rstar, cashflows, times, k, sigma, ravg, expiry, strike):
    total_price = 0.0
    for cfcount in range(0,len(cashflows)):
        total_price += cashflows[cfcount] * price_discount_bond_explicit(rstar, sigma, k, ravg, expiry, times[cfcount],1)
    return(total_price-strike)


def calculate_exp_price_iter(r_star, sigma, k, ravg, expiry, time, r0, cf):
    strike_star = price_discount_bond_explicit(r_star, sigma, k, ravg, expiry, time, 1)

    sigma_p = (sigma / k) * (1 - math.exp(-k * (time - expiry))) \
              * math.sqrt((1 - math.exp(-2 * k * expiry)) / (2 * k))

    price_1 = price_discount_bond_explicit(r0, sigma, k, ravg, 0, time, 1)
    price_2 = price_discount_bond_explicit(r0, sigma, k, ravg, 0, expiry, 1)

    d_p = (1 / sigma_p) * math.log(price_1 / (strike_star * price_2)) + sigma_p / 2
    d_n = (1 / sigma_p) * math.log(price_1 / (strike_star * price_2)) - sigma_p / 2

    return cf * (price_1 * st.norm.cdf(d_p) - strike_star * price_2 * st.norm.cdf(d_n))

