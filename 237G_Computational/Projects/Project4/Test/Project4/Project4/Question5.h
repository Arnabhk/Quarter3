#include "Generators.h"
#include "Helpers.h"
#pragma once
using namespace std;

void Question5();
double PriceTrinomial(int noOfSteps, double r, double delta, double s0, double u, double d, double k, double pd, double pm, double pu, bool logNormal=false);
void BuildStockPrice(int noOfSteps, double** stockPrices, double u, double d, bool logNormal = false);
void BuildCallPrice(int noOfSteps, double** callPrices, double** stockPrices, double r, double delta, double k, double pu, double pm, double pd, bool logNormal);
void CleanUp(double** prices, double** callPrices, int noOfSteps);