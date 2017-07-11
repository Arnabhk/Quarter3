#define _USE_MATH_DEFINES
#include <cmath>
#include "Question3.h"
#include <algorithm>
#include <Windows.h>
using namespace std;

Question3::Question3(double seed)
{
	_seed = seed;
	_total = 10000;
}

vector<double> Question3::RunQ3(double S_0, double T, double X, double r, double Sigma)
{
	//Antithetic 
	vector<double> prices_varRed = EurpnCall_MonteCarlo_Antithetic(S_0, T, X, r, Sigma);
	cout << endl << "Mean price after antithetic variance reduction = " << mean(prices_varRed) <<endl;
	cout << "Standard Deviation price after antithetic variance reduction = " << stddev(prices_varRed) <<endl;

	//Black Scholes price
	double bsPrice = BlackScholes_Call(S_0, T, X, r, Sigma);
	cout << endl << "Mean pricing using standard black scholes = " << bsPrice <<endl;

	//Hedging Parameters
	CalculateAndPlotGreeks(X,Sigma,r,T);

	vector<double> answers{ mean(prices_varRed), bsPrice };
	return answers;
}

vector<double> Question3::EurpnCall_MonteCarlo_Antithetic(double S_0, double T, double X, double r, double Sigma)
{
	vector<double> randNums_normal = GenerateNormalDistn(_total, _seed);
	vector<double> prices = EurpnCall_Prices(T, S_0, r, Sigma, _total, X, randNums_normal);

	//Take -1 of the randNums and use it for call option price calculation
	vector<double> randNums_varRed(randNums_normal.size());
	copy(randNums_normal.begin(), randNums_normal.end(), randNums_varRed.begin());
	for_each(randNums_varRed.begin(), randNums_varRed.end(), [](double& n) {n = -1.0 * n; });
	vector<double> prices_varRed = EurpnCall_Prices(T, S_0, r, Sigma, _total, X, randNums_varRed);
	
	vector<double> combinedPrc = GenerateVarianceReductionDistbn(prices, prices_varRed);

	return(combinedPrc);
}

vector<double> Question3::EurpnCall_Prices(double T, double S_0, double r, double Sigma, int N, double X, vector<double>randNums)
{
	vector<double> prices;
	vector<double> St = GenerateTerminalPrice(T, S_0, r, Sigma, N, randNums);
	for (int i = 0; i < _total; i++)
	{
		double payoff = St[i] - X > 0 ? St[i] - X : 0;
		prices.push_back(exp(-1.0*r*T)*payoff);
	}
	
	return prices;
}

vector<double> Question3::GenerateVarianceReductionDistbn(vector<double> val1, vector<double> val2)
{
	vector<double> val_combined(val1.size());
	transform(val1.begin(), val1.end(), val2.begin(), val_combined.begin(), std::plus<double>());
	for_each(val_combined.begin(), val_combined.end(), [](double& n) {n = n / 2.0; });
	return(val_combined);
}


vector<double> Question3::GenerateTerminalPrice(double T, double S0, double r, double sigma, int NoOfPaths, vector<double> randNums)
{
	vector<double> stockPrices;

	for (int count = 0; count < NoOfPaths; count++)
	{
		double St = S0 * exp((r - pow(sigma, 2) / 2)*T + sigma*sqrt(T)*randNums[count]);
		stockPrices.push_back(St);
	}
	return stockPrices;
}

double Question3::BlackScholes_Call(double S0, double T, double X, double r, double sig)
{
	double d1 = (log(S0 / X) + (r + pow(sig, 2) / 2)*T) / (sig*sqrt(T));
	double d2 = d1 - sig*sqrt(T);

	double test = exp(-1.0 * r * T);
	double price = S0 * NCap(d1) - X * test * NCap(d2);

	return(price);
}
double Question3::NCap(double x)
{
	if (x < 0)
	{
		return 1 - NCap(-1.0 * x);
	}

	// constants
	double d1 = 0.0498673470;
	double d2 = 0.0211410061;
	double d3 = 0.003277623;
	double d4 = 0.0000380036;
	double d5 = 0.0000488906;
	double d6 = 0.0000053830;

	// Save the sign of x
	double val = 1 + d1*x + d2*pow(x, 2)+ d3*pow(x, 3) + d4*pow(x, 4) 
				+ d5*pow(x,5) + d6*pow(x, 6);
	return 1.0 - 0.5 * pow(val, -1.0 * 16);
}

double Question3::NSmall(double x)
{
	return ((1 / sqrt(2 * M_PI))*exp(-1.0 * pow(x, 2) / 2));
}

void Question3::CalculateAndPlotGreeks(double X, double Sigma, double r, double Time)
{
	vector<double> deltas, gammas, thetas, vegas, rhos;

	for (double S0 = 15; S0 <= 25; S0++)
	{
		double d1 = (log(S0 / X) + (r + pow(Sigma, 2) / 2)*Time) / (Sigma*sqrt(Time));
		double d2 = d1 - Sigma*sqrt(Time);

		//Delta
		deltas.push_back(NCap(d1));

		//Gamma
		double gamma = NSmall(d1) / (S0*Sigma*sqrt(Time));
		gammas.push_back(gamma);

		//Theta
		double theta = (-1.0*S0*Sigma*NSmall(d1) / (2 * sqrt(Time))) - (r * X * exp(-1.0 * r * Time) * NCap(d2));
		thetas.push_back(theta);

		//Vega
		double vega = S0 * sqrt(Time) * NSmall(d1);
		vegas.push_back(vega);

		//RHo
		double rho = X * Time * exp(-1.0 * r * Time) * NCap(d2);
		rhos.push_back(rho);
	}

	CreateDirectoryA("Excels", NULL);
	writeToExcel("Excels/deltas.csv", deltas);
	writeToExcel("Excels/gammas.csv", gammas);
	writeToExcel("Excels/thetas.csv", thetas);
	writeToExcel("Excels/vegas.csv", vegas);
	writeToExcel("Excels/rhos.csv", rhos);

}



