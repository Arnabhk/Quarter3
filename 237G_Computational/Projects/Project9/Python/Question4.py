import Question1


def question4(market_price, oas):
    y = 0.0005
    wac = 0.08
    loan = 100000
    r0 = 0.078
    k = 0.6
    ravg = 0.08
    sigma = 0.12
    time = 30
    no_of_sim = 1000

    price_plus = Question1.price_mbs(wac, loan, r0, k, ravg, sigma, time, "NUMERIX", no_of_sim, oas + y)

    price_minus = Question1.price_mbs(wac, loan, r0, k, ravg, sigma, time, "NUMERIX", no_of_sim, oas - y)

    duration = (price_minus - price_plus)/(2*y*market_price)
    convexity = (price_plus + price_minus - 2*market_price)/(2* market_price * (y**2))

    print("OAS Duration is {0:.4f}".format(duration))
    print("OAS Convexity is {0:.4f}".format(convexity))