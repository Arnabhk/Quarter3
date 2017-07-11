#include "Question1.h"
#include <algorithm>
#include <math.h>
#include <random>

using namespace std;

void Question1()
{
	double S0 = 98;
	double strike = 100;
	double r = 0.03;
	double T = 1;
	double stepSize = 0.001;
	double seed = 2000;
	double NoOfPaths = 2000;

	vector<double> callPrices, putPrices;
	for (double vol = 0.12; vol <= 0.48; vol = vol + 0.04)
	{
		vector<double> callPayoffs, putPayoffs;
		for (int count = 0; count < NoOfPaths; count++) 
		{
			vector<double> stockPath = GenerateFullStockPath(T, S0, r, vol, stepSize, int(seed*count));
			double maxPrice = *max_element(stockPath.begin(), stockPath.end());
			double minPrice = *min_element(stockPath.begin(), stockPath.end());
			callPayoffs.push_back(max(maxPrice - strike, 0.0) * exp(-1.0 * r * T));
			putPayoffs.push_back(max(strike - minPrice, 0.0) * exp(-1.0 * r * T));
		}
		
		callPrices.push_back(mean(callPayoffs));
		putPrices.push_back(mean(putPayoffs));
		
	}
	writeToConsole(callPrices);
	writeToConsole(putPrices);

	writeToExcel("callPrices.csv", callPrices);
	writeToExcel("putPrices.csv", putPrices);
}




