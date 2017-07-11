import Question1
import scipy.optimize as opt


def question3(market_price):
    wac = 0.08
    loan = 100000
    r0 = 0.078
    k = 0.6
    ravg = 0.08
    sigma = 0.12
    time = 30
    no_of_sim = 1000

    oas = opt.newton(calculate_error, 0, args=(market_price, wac, loan, r0, k, ravg, sigma, time, no_of_sim))
    print("The OAS for the MBS is {0:.4f}".format(oas))
    return oas

def calculate_error(x, market_price, wac, loan, r0, k, ravg, sigma, time, no_of_sim):
    #np.random.seed(1234)
    mean_price = Question1.price_mbs(wac, loan, r0, k, ravg, sigma, time, "NUMERIX", no_of_sim, x,"None")

    return mean_price - market_price