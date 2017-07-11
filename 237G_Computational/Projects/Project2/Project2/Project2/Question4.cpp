#define _USE_MATH_DEFINES
#include <cmath>
#include<math.h>
#include "Question4.h"
using namespace std;

vector<double> Question4(double seed)
{
	double r = 0.04;
	double sig = 0.2;
	double S0 = 88;
	double T = 5;
	double X = 100;
	
	vector<double> answers;

	//a
	int N = 10000;
	//Get Normal distribution
	vector<double> randNums_normal = GenerateNormalDistn(N, seed);
	
	//Generate Stock Path and run monte carlo to get call prices
	vector<double> St = GenerateStockPath(T,S0,r,sig,N,randNums_normal);
	vector<double> callPrc = MonteCarlo_EuropeanOpt(N, St, X, T, r);
	double callPriceSim_mean = mean(callPrc);
	double callPrcSim_sd = stddev(callPrc);
	cout << "a)" << endl;
	cout << "Call Price using simulation = " << callPriceSim_mean << endl;
	answers.push_back(callPriceSim_mean);

	//b - Blackscholes
	double bs_c = BlackScholes_Call(S0, X, T, r, sig);
	cout << "b) " << endl;
	cout << "Call Price using Black Scholes = " << bs_c << endl;
	answers.push_back(bs_c);

	cout << endl << "For Variance reduction, we use the antithetic method." << endl;
	cout << "1) Calculate call price (c2) using -z (negative of random numbers)" <<endl;
	cout << "2) Set (c1(original)+c2)/2 as new set of prices and calculate mean and standard deviation" << endl;
	vector<double> randNums_varRed(randNums_normal.size());
	copy(randNums_normal.begin(), randNums_normal.end(), randNums_varRed.begin());
	for_each(randNums_varRed.begin(), randNums_varRed.end(), [](double& n) {n = -1.0 * n; });

	vector<double> st2 = GenerateStockPath(T, S0, r, sig, N, randNums_varRed);
	vector<double> callPrc2 = MonteCarlo_EuropeanOpt(N, st2, X, T, r);

	vector<double> callPrc_VarRed = GenerateVarianceReductionDistbn(callPrc, callPrc2);
	double callPrcVarRed_mean = mean(callPrc_VarRed);
	double callPrcVarRed_sd = stddev(callPrc_VarRed);
	cout << endl<< "Call Price using variance reduction = " << callPrcVarRed_mean << endl;
	answers.push_back(callPrcVarRed_mean);

	cout << "The Standard deviation using normal simulation = " << callPrcSim_sd <<endl;
	cout << "The standard deviation after antithetic variance reduction =" << callPrcVarRed_sd << endl;
	cout << endl<< "As it can be seen, the standard deviation is reduced by this method. This is because the covariance between the variables is negative." << endl;
	cout << "This makes the average of the 2 variables have lesser covariance than just one of them." << endl;
	
	return(answers);
}

vector<double> MonteCarlo_EuropeanOpt(int NoOfPaths, vector<double> St, double X, double T, double r)
{
	vector<double> prices;
	for (int i = 0; i < NoOfPaths; i++)
	{
		double payoff = St[i] - X > 0 ? St[i]-X : 0;
		prices.push_back(exp(-1.0*r*T)*payoff);
	}
	return prices;

}


double BlackScholes_Call(double S0, double X, double T, double r, double sigma)
{
	double d1 = (log(S0 / X) + (r + pow(sigma,2)/2)*T)/(sigma*sqrt(T));
	double d2 = d1 - sigma*sqrt(T);

	double test = exp(-1.0 * r * T);
	double price = S0 * Normal_Pdf(d1) - X * test * Normal_Pdf(d2);

	return(price);
}

double Normal_Pdf(double x)
{
	// constants
	double a1 = 0.254829592;
	double a2 = -0.284496736;
	double a3 = 1.421413741;
	double a4 = -1.453152027;
	double a5 = 1.061405429;
	double p = 0.3275911;

	// Save the sign of x
	int sign = 1;
	if (x < 0)
		sign = -1;
	x = fabs(x) / sqrt(2.0);

	// A&S formula 7.1.26
	double t = 1.0 / (1.0 + p*x);
	double y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);

	return 0.5*(1.0 + sign*y);
}