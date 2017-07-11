#include "Question1.h"
#include <algorithm>
#include <math.h>

using namespace std;

void Question1()
{
	double sigma = 0.24;
	double r = 0.05;
	double s0 = 32;
	double K = 30;
	double time = 0.5;

	vector<int> noOfSteps{ 10,20,40,80,100,200,500 };
	vector<vector<double>> answers;
	
	//Part a
	vector<double> answersa, answersb, answersc, answersd;
	for (int count = 0; count < noOfSteps.size(); count++)
	{
		double delta = time / noOfSteps[count];
		double ansa = Parta(r, delta, sigma, noOfSteps[count], s0, K);
		answersa.push_back(ansa);

		double ansb = Partb(r, delta, sigma, noOfSteps[count], s0, K);
		answersb.push_back(ansb);

		double ansc = Partc(r, delta, sigma, noOfSteps[count], s0, K);
		answersc.push_back(ansc);

		double ansd = Partd(r, delta, sigma, noOfSteps[count], s0, K);
		answersd.push_back(ansd);
	}
	answers.push_back(answersa);
	answers.push_back(answersb);
	answers.push_back(answersc);
	answers.push_back(answersd);

	//To plot graphs
	writeToExcel("Q1.csv",answers);
	//
	
	cout << endl << "All the prices for the 4 types and various values of N are"<<endl;
	writeToConsole(answers);

	//Convergence value
	double bs_callPrice = BlackScholes_Call(s0, time, K, r, sigma);
	cout << endl << "Convergence price ,i.e., Black Scholes price = " << bs_callPrice << endl;
}

double Parta(double r, double delta, double sigma, int noOfSteps, double s0, double K)
{
	double c = (exp(-1.0 * r * delta) + exp((r + pow(sigma, 2))*delta)) / 2.0;
	double d = c - sqrt(pow(c, 2) - 1);
	double u = 1.0 / d;
	double prob = (exp(r*delta) - d) / (u - d);
	return PriceOption(r, noOfSteps, delta, prob, u, d, s0, K);
}

double Partb(double r, double delta, double sigma, int noOfSteps, double s0, double K)
{
	double d = exp(r*delta)*(1 - sqrt(exp(pow(sigma, 2)*delta) - 1));
	double u = exp(r*delta)*(1 + sqrt(exp(pow(sigma, 2)*delta) - 1));
	double prob = 0.5;
	return PriceOption(r, noOfSteps, delta, prob, u, d, s0, K);
}

double Partc(double r, double delta, double sigma, int noOfSteps, double s0, double K, char optionType)
{
	double d = exp((r - (pow(sigma, 2) / 2))*delta - sigma*sqrt(delta));
	double u = exp((r - (pow(sigma, 2) / 2))*delta + sigma*sqrt(delta));
	double prob = 0.5;
	return PriceOption(r, noOfSteps, delta, prob, u, d, s0, K, optionType);
}

double Partd(double r, double delta, double sigma, int noOfSteps, double s0, double K)
{
	double d = exp(-1.0 * sigma*sqrt(delta));
	double u = exp(sigma*sqrt(delta));
	double prob = 0.5 + 0.5 * ((r - (pow(sigma, 2) / 2))*sqrt(delta)) / sigma;
	return PriceOption(r, noOfSteps, delta, prob, u, d, s0, K);
}

double PriceOption(double r, int noOfSteps, double delta, double prob, double u, double d, double s0, double K, char optionType)
{
	double price = 0.0;
	for (int count = 0; count <= noOfSteps; count++)
	{
		double st = s0 * pow(u, count) * pow(d, noOfSteps - count);
		double priceVal = optionType == 'c' ? max(st - K, 0.0) : max(K - st, 0.0);
		double bnlProb = BnlProb(noOfSteps, count, prob);
		price += bnlProb*priceVal;
	}
	return (exp(-1.0 * r * noOfSteps * delta) * price);
}

double BnlProb(unsigned n, unsigned k, double prob)
{
	return exp(lgamma(n + 1) - lgamma(k + 1) - lgamma(n - k + 1))*pow(prob, k)*pow(1 - prob, n - k);
}

double BlackScholes_Call(double S0, double T, double X, double r, double sig)
{
	double d1 = (log(S0 / X) + (r + pow(sig, 2) / 2)*T) / (sig*sqrt(T));
	double d2 = d1 - sig*sqrt(T);

	double test = exp(-1.0 * r * T);
	double price = S0 * NCap(d1) - X * test * NCap(d2);

	return(price);
}


double NCap(double x)
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
	double val = 1 + d1*x + d2*pow(x, 2) + d3*pow(x, 3) + d4*pow(x, 4)
		+ d5*pow(x, 5) + d6*pow(x, 6);
	return 1.0 - 0.5 * pow(val, -1.0 * 16);
}


