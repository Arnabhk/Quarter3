#pragma once
#include<vector>
#include<algorithm>

using namespace std;

vector<double> GenerateNormalDistn(int N, double seed);
vector<double> GenerateUniformDist(int N, double seed);
vector<double> GenerateStockPath(double T, double S0, double r, double sigma, int NoOfPaths, vector<double> randNums);
vector<double> GenerateFullStockPath(double T, double S0, double r, double sigma, double stepSize, double seed);
vector<double> GenerateVarianceReductionDistbn(vector<double> val1, vector<double> val2);