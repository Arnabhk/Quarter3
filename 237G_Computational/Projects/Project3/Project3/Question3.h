#pragma once
#include "Generators.h"
#include "Helpers.h"

using namespace std;

class Question3
{
public:
	Question3(double seed);
	vector<double> RunQ3(double S_0, double T, double X, double r, double Sigma);

private:
	double _seed;
	int _total;

	vector<double> EurpnCall_MonteCarlo_Antithetic(double S_0, double T, double X, double r, double Sigma);
	vector<double> GenerateTerminalPrice(double T, double S0, double r, double sigma, int NoOfPaths, vector<double> randNums);
	vector<double> EurpnCall_Prices(double T, double S_0, double r, double Sigma, int N, double X, vector<double>randNums);
	vector<double> GenerateVarianceReductionDistbn(vector<double> val1, vector<double> val2);
	double BlackScholes_Call(double S0, double T, double X, double r, double sig);
	double NCap(double x);
	double NSmall(double x);
	void CalculateAndPlotGreeks(double X, double Sigma, double r, double T);
};
