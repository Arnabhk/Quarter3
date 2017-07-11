#pragma once
#include "Helpers.h"

void Question5(double seed);
float* GetNormalDistWithStats(int N, double seed, bool printStats);
std::vector<double> GenerateNormalDistn_BoxMuller(int N, double seed);
std::vector<double> GenerateNormalDistn_PolarMarsaglia(int N, double seed);