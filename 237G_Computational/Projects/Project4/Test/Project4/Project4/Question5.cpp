#define _USE_MATH_DEFINES
#include <cmath>
#include "Question5.h"
#include "Question1.h"
using namespace std;

void Question5()
{
	double time = 0.5;
	double r = 0.05;
	double sigma = 0.24;
	double s0 = 32;
	double k = 30;

	vector<int> N{ 10, 15, 20, 40, 70, 80, 100, 200, 500 };

	vector<double> answersa;
	vector<double> answersb;

	for (int count = 0; count < N.size(); count++)
	{
		double delta = time / N[count];
		
		//Part a
		double d_a = exp(-1.0 * sigma * sqrt(3 * delta));
		double u_a = 1 / d_a;
		double pd = (r * delta * (1 - u_a) + pow(r*delta, 2) + pow(sigma, 2)*delta) / ((u_a - d_a) * (1 - d_a));
		double pu = (r * delta * (1 - d_a) + pow(r*delta, 2) + pow(sigma, 2)*delta) / ((u_a - d_a) * (u_a - 1));
		double pm = 1 - pd - pu;

		double val = PriceTrinomial(N[count], r, delta, s0, u_a, d_a, k, pd, pm, pu);
		answersa.push_back(val);

		//Part b
		double d_b = -1.0 * sigma * sqrt(3 * delta);
		double u_b = sigma * sqrt(3 * delta);
		double varnce = (pow(sigma, 2));
		double drift = (r - (varnce/ 2));
		double pd_b = 0.5 * (((varnce*delta + pow(drift,2)*pow(delta,2))/pow(u_b,2)) - ((drift *delta)/u_b));
		double pu_b = 0.5 * (((varnce*delta + pow(drift, 2)*pow(delta, 2)) / pow(u_b, 2)) + ((drift *delta) / u_b));
		double pm_b = 1 - pd_b - pu_b;

		double val_b = PriceTrinomial(N[count], r, delta, log(s0),u_b, d_b, k, pd_b, pm_b, pu_b, true);
		answersb.push_back(val_b);
	}

	vector<vector<double>> answers;
	answers.push_back(answersa);
	answers.push_back(answersb);
	writeToExcel("Q5.csv", answers);

	//Convergence
	double blackScholesVal = BlackScholes_Call(s0, time, k, r, sigma);
	cout << endl << "The Trinomial returns for the call option is" << endl;
	writeToConsole(answers);
	cout << endl << "Convergence Price, i.e., Black scholes price = " << blackScholesVal << endl;
}

double PriceTrinomial(int noOfSteps, double r, double delta,double s0, double u, double d, double k, double pd, double pm, double pu, bool logNormal)
{
	//Initialize array
	double** stockPrices = new double*[(2 * noOfSteps) + 1];
	double** callPrices = new double*[(2 * noOfSteps) + 1];
	for (int count = 0; count <= 2*noOfSteps; count++)
	{
		stockPrices[count] = new double[noOfSteps + 1]{ 0 };
		callPrices[count] = new double[noOfSteps + 1]{ 0 };
	}

	//Populate stockprices
	stockPrices[2*noOfSteps][0] = s0;
	BuildStockPrice(noOfSteps, stockPrices, u, d, logNormal);
	//Call Prices
	BuildCallPrice(noOfSteps, callPrices, stockPrices, r, delta, k, pu, pm, pd, logNormal);
	double callPrice = callPrices[2 * noOfSteps][0];
	CleanUp(stockPrices, callPrices, 2*noOfSteps);
	return callPrice;
}

void CleanUp(double** prices, double** callPrices, int noOfSteps)
{
	for (int count = 0; count < noOfSteps; count++)
	{
		delete[] prices[count];
		delete[] callPrices[count];
	}
	delete[] prices;
	delete[] callPrices;
}

void BuildStockPrice(int noOfSteps, double** stockPrices, double u, double d, bool logNormal)
{
	for (int count = 1; count <= noOfSteps; count++)
	{
		for (int j = 1; j <= ((2 * count) - 1); j++)
		{
			stockPrices[(2*noOfSteps) - j][count] = stockPrices[2 * noOfSteps - j + 1][count - 1];
		}
		stockPrices[(2 * noOfSteps)][count] = logNormal ? stockPrices[2*noOfSteps][count - 1] + d
													: stockPrices[2*noOfSteps][count - 1] * d;
		stockPrices[(2 * noOfSteps) - (2 * count)][count] = logNormal ? stockPrices[2*noOfSteps - (2 * count) + 2][count - 1] + u
													 :stockPrices[2*noOfSteps - (2 * count) + 2][count - 1] * u;
	}
}

void BuildCallPrice(int noOfSteps, double** callPrices, double** stockPrices, double r, double delta, double k, double pu, double pm, double pd, bool logNormal)
{
	for (int count = 0; count <= 2*noOfSteps; count++)
	{
		double stPrice = logNormal ? exp(stockPrices[count][noOfSteps]) : stockPrices[count][noOfSteps];
		callPrices[count][noOfSteps] = stPrice - k >= 0 ? stPrice - k: 0;
	}

	for (int colcount = noOfSteps - 1; colcount >= 0; colcount--)
	{
		for (int j = 0; j <= (2 * colcount); j++)
		{
			callPrices[2 * noOfSteps - j][colcount] = (pu * callPrices[2*noOfSteps - j - 2][colcount + 1]
				+ pm * callPrices[2*noOfSteps - j - 1][colcount + 1]
				+ pd * callPrices[2*noOfSteps - j][colcount + 1]) * exp(-1.0 * r * delta);
		}
	}
}