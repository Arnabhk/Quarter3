#pragma once
#include "Generators.h"
#include "Helpers.h"
#include <sstream>

using namespace std;

void Question5(double seed);
vector<double> CalculateMeanOfPaths(double S0, double r, double sigma, int NoOfSim, double seed);
vector<vector<double>> CalculateAllStockPaths(double S0, double r, double sigma, double Time, double seed, double stepSize, int noOfPaths);