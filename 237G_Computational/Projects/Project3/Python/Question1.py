import statistics
import numpy as np

def question1(seed):

    #a
    no_of_sim = 1000
    dt = 0.01
    time = 2
    no_of_step = int(time/dt)
    x0 = 1

    x_path = np.zeros((no_of_sim,(no_of_step+1)))
    x_path[:,0] = x0

    np.random.seed(seed)
    rand = np.random.normal(0, 1, size=(no_of_sim, no_of_step))

    for count in range(1,no_of_step+1):
        dt_term = (0.2 - 0.5 * x_path[:,count - 1])*dt
        wt_term = (2.0 / 3.0) * rand[:,count - 1] * np.sqrt(dt)
        x_path[:,count] = x_path[:,count - 1] + dt_term + wt_term

    ans_b = statistics.mean([val**(1.0/3.0) if val>=0 else
                             -((-val)**(1.0/3.0))
                             for val in x_path[:,no_of_step]])
    print("X2^(1/3) = {0:.4f}".format(ans_b))





