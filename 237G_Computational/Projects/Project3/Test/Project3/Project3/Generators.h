#pragma once
#include "Generators.h"
#include <vector>
using namespace std;

vector<double> GenerateNormalDistn(int N, double seed);
vector<double> GenerateUniformDist(int N, double seed);
void GenerateBiVariateNormalDistn(int n, double seed, double mu[], double sd[], double cor, vector<double> &x1, vector<double> &x2);