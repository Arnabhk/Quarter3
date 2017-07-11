#pragma once
#include "Generators.h"
#include <vector>
using namespace std;

vector<double> GenerateFullStockPath(double T, double S0, double r, double sigma, double stepSize, double seed, bool logPrice = false);
vector<double> GenerateNormalDistn(int N, double seed);
vector<double> GenerateExpDistn(int N, double seed, double lambda);
vector<double> GenerateUniformDist(int N, double seed);
vector<double> GeneratePoissonDist(int N, double gamma, double stepSize, double seed);
