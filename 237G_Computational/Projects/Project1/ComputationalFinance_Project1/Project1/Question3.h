#pragma once
#include "Helpers.h"

void Question3(int n, double prob, int total, double seed);
std::vector<int> BuildBinomialDist(int n, double prob, int total,double seed);
double RealBnlProb(unsigned n, unsigned k, double prob);
unsigned nChoosek(unsigned n, unsigned k);