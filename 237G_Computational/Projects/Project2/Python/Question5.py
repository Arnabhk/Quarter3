import matplotlib.pyplot as plot
import numpy.random as rand
import numpy as np

def question5(seed):
    s0 = 88
    r = 0.04
    sig = 0.18
    dt = 1/100
    no_of_sim = 1000
    T = 10
    rand.seed(seed)
    stock_path = build_stock_path(s0, r, sig, T, dt, no_of_sim)

    plot.figure("Figure1")
    plot.plot(np.arange(0,T+dt,dt), stock_path[1, :], 'r--', linewidth=1)
    plot.plot(np.arange(0, T + dt, dt), stock_path[2, :], 'g--', linewidth=1)
    plot.show()

def build_stock_path(s0, r, sig, T, dt,no_of_sim):
    no_of_steps = int(T/dt)
    rand_nums = np.matrix(rand.standard_normal(no_of_sim*no_of_steps)).reshape(no_of_sim,no_of_steps)
    time = np.arange(1,no_of_steps+1)

    stock_path = np.zeros((no_of_sim,no_of_steps+1))
    stock_path[:,0] = s0
    stock_path[:,1:no_of_steps+1] = s0 * np.exp(np.matrix((r - (sig**2)/2)*np.transpose(time)*dt) +
                                              np.matrix((sig * np.sqrt(dt))*np.cumsum(rand_nums,1)))

    return stock_path