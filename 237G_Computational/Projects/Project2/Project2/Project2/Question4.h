#pragma once
#include "Generators.h"
#include "Helpers.h"
using namespace std;

vector<double> Question4(double seed);
vector<double> MonteCarlo_EuropeanOpt(int N, vector<double> st, double X, double T, double r);
double BlackScholes_Call(double S0, double X, double T, double r, double sigma);
double Normal_Pdf(double x);