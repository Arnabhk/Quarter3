#pragma once
#include "Generators.h"
#include "Helpers.h"
using namespace std;

vector<double> Question3(double seed);
vector<double> MonteCarloQ3_1(vector<double> z, int NoOfSim);
vector<double> MonteCarloQ3_2(vector<double> z, int NoOfSim, double t);
vector<double> MonteCarloQ3_1_VarRed(vector<double> z, int NoOfSim, vector<double> values, double gamma);
vector<double> MonteCarloQ3_2_VarRed(vector<double> z, double t, int NoOfSim, vector<double> values, double gamma);
