#pragma once
#include "Generators.h"
#include "Helpers.h"

using namespace std;

vector<double> Question4(double seed1, double seed2);
vector<double> ConstructStockPath(double r, double time, vector<double> volatility, vector<double> randNums, double S0, char* method);
vector<double> ConstructVolPath(double a, double b, double sig, double time, vector<double> randNums, double V0, char* method);
double positiveCutoff(double n);
double same(double n);
double absVal(double n);
double optionPayoff(double s, double k);