#pragma once
#include "Helpers.h"
#include "Generators.h"
using namespace std;

void Proj6_2func();
vector<double> CalculateDefaultInfo(double lambda1 = 0.2, double lambda2 = 0.4, double T = 5.0);
vector<double> GenerateCollPricePath_Method2(double v0, double mu, double sigma, double gamma, double lambda1, double stepSize, double time, double pathNo, double seed);
vector<double> GenerateCollPricePath_Method1(double v0, double mu, double sigma, double gamma, double lambda1, double stepSize, double time, double pathNo, double seed);
vector<double> GenerateLoanPath(double L0, double r0, double delta, double lambda2, double stepSize, double time);
double CalculateQStopping(vector<double> collPricePath, vector<double> loanPath, vector<double> qt, double stepSize);
vector<double> GenerateLoanFrac(double alpha, double beta, double stepSize, double time);
double CalculateSStopping(double seed, int pathCount, double lambda2);
double CalculateTau(double stoppingQ, double stoppingS, double time);
double CalculatePayoff(double stoppingQ, double stoppingS, double tau, double stepSize, double epsilon, vector<double> collPricePath, vector<double> loanPath);