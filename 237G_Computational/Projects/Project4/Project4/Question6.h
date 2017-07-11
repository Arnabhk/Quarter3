#pragma once
#include "Helpers.h"
#include "Generators.h"

using namespace std;

void Question6();
vector<double> HaltonSequence(int k, int m);
double PriceCall_Halton(double s0, double k, double time, double r, double sigma, double N, double b1, double b2);
vector<double> GenerateNormalDistn_BM(int N, double b1, double b2);


