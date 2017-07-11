#pragma once

#include "Generators.h"
#include "Helpers.h"
vector<double> Question6(double seed);
double EulerDiscretization(double h, int a, int b);
vector<double> MonteCarloIntegration(int NoOfSim, double seed);
vector<double> ImportanceSampling(int NoOfSim, double seed);