import Question1
import numpy as np
import matplotlib.pyplot as plot


def question3():
    k = 50;
    r = 0.03;
    sigma = 0.2;
    time = 0.3846;

    #Parta
    noOfStepsa = 100;
    deltaa = time / noOfStepsa;
    s0_all = np.array(range(20,82,2))
    deltas = [0] * len(s0_all)
    for count in range(0,len(s0_all)):
        c0 = Question1.Partc(r, deltaa, sigma, noOfStepsa, s0_all[count], k);
        su = s0_all[count] + 0.01;
        cu = Question1.Partc(r, deltaa, sigma, noOfStepsa, su, k);
        deltas[count] = (cu - c0) / (su - s0_all[count])

    plot.figure("Figure 1")
    plot.plot(s0_all,deltas)
    plot.show()


    #Partb
    s0 = 49;
    deltaTime = 0.01;
    noOfStepsb = 1000;
    time_all = np.arange(0,time+deltaTime, deltaTime)
    time_based = [0] * len(time_all)
    for count in range(0,len(time_based)):
        c0 = Question1.Partc(r, time_all[count]/noOfStepsb, sigma, noOfStepsa, s0, k);
        su = s0 + 0.01;
        cu = Question1.Partc(r, time_all[count]/noOfStepsb, sigma, noOfStepsa, su, k);
        time_based[count] = (cu - c0) / (su - s0)

    plot.figure("Figure 1")
    plot.plot(time_all, time_based)

    #Part c
    deltac = 0.0005;
    noOfStepsc = (int)(time / deltac)
    s0_all = np.array(range(20, 82, 2))
    thetas = [0] * len(s0_all)
    for count in range(0, len(s0_all)):
        c0 = Question1.Partc(r, deltac, sigma, noOfStepsc, s0_all[count], k);
        cu = Question1.Partc(r, deltac, sigma, noOfStepsc - 5, s0_all[count], k);
        thetas[count] = (cu - c0) / (5 * deltac)

    plot.figure()
    plot.plot(s0_all, thetas)


    # Part d
    deltad = 0.001;
    noOfStepsd = (int)(time / deltad);
    s0_all = np.array(range(20, 82, 2))
    gammas = [0] * len(s0_all)
    for count in range(0, len(s0_all)):
        c0 = Question1.Partc(r, deltad, sigma, noOfStepsd, s0_all[count], k);
        su = s0_all[count] + 1;
        cu = Question1.Partc(r, deltad, sigma, noOfStepsd, su, k);
        deltau = (cu - c0) / (su - s0_all[count]);
        suu = su + 1;
        cuu = Question1.Partc(r, deltad, sigma, noOfStepsd, suu, k);
        deltauu = (cuu - cu) / (suu - su);
        gammas[count] = (deltauu - deltau) / ((suu - s0_all[count]) / 2.0);

    plot.figure()
    plot.plot(s0_all, gammas)

