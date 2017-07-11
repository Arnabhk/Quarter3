#pragma once
#include "Helpers.h"
#include "Generators.h"
using namespace std;

void Question2();
void RetrievePrices(vector<double>& prices, vector<int>& years);
double ClosestMultiple(double x);
vector<double> CalculateAnnualReturns(vector<double> prices, vector<int> years);
double CalculateImpliedVol(double sigma, double delta, double r, double N, double s0, double k, double market_call);
