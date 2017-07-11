#pragma once
#include "Generators.h"
#include "Helpers.h"

using namespace std;

double Question1(double seed);
void GenerateBiVariateNormalDistn(int n, double seed, double mu[], double sd[], double cor, vector<double>& x1, vector<double>& x2);

