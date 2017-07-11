#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include "Question6.h"
#include "Question1.h"

void Question6()
{
	double s0 = 32;
	double k = 30;
	double time = 0.5;
	double r = 0.05;
	double sigma = 0.24;
	double N = 1000;
	double b1 = 2;
	double b2 = 7;
	double price = PriceCall_Halton(s0, k, time, r, sigma, N, b1, b2);
	cout << endl << "Price of call option using halton sequenes (b1=2, b2=7) = "<<price<<endl;

	double blackScholesVal = BlackScholes_Call(s0, time, k, r, sigma);
	cout << endl << "Convergence Price, i.e., Black scholes price = " << blackScholesVal << endl;
}

double PriceCall_Halton(double s0, double k, double time, double r, double sigma, double N, double b1, double b2)
{
	vector<double> haltonSeq = GenerateNormalDistn_BM(N, b1, b2);

	vector<double> terminalPrices;
	for (int count = 0; count < N; count++)
	{
		double tprice = s0 * exp((r - (pow(sigma, 2) / 2.0)) * time + (sigma * sqrt(time) * haltonSeq[count]));
		terminalPrices.push_back(max(tprice - k, 0.0));
	}

	return exp(-1.0 * r * time) * mean(terminalPrices);
}

vector<double> GenerateNormalDistn_BM(int N, double b1, double b2)
{
	vector<double> randNums_1 = HaltonSequence((int)N / 2, b1);
	vector<double> randNums_2 = HaltonSequence((int)N / 2, b2);

	vector<double> normalDist;

	for (int count = 0; count < randNums_1.size(); count++)
	{
		double z1 = sqrt(-2 * log(randNums_1[count]))*cos(2 * M_PI * randNums_2[count]);
		double z2 = sqrt(-2 * log(randNums_1[count]))*sin(2 * M_PI * randNums_2[count]);
		normalDist.push_back(z1);
		normalDist.push_back(z2);
	}

	return normalDist;
}

vector<double> HaltonSequence(int k, int m)
{
	vector<double> sequence(k, 0);
	double r = ceil(log(k) / log(m)) + 1;
	vector<double> baseRep(r, 0);
	for (int count = 0; count < k; count++)
	{
		int j = 0;
		bool done = false;
		while (!done)
		{
			baseRep[j] = baseRep[j] + 1;
			if (baseRep[j] < m) { done = true; }
			else
			{
				baseRep[j] = 0;
				j++;
			}
		}
		for (int baseCt = 0; baseCt < r; baseCt++)
		{
			sequence[count] = sequence[count] + (baseRep[baseCt] / pow(m, baseCt + 1));
		}
	}
	return sequence;
}