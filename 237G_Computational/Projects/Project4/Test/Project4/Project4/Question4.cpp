#include "Question1.h"
#include "Question4.h"
#include <algorithm>
using namespace std;

void Question4()
{
	double time = 1;
	double k = 100;
	double sigma = 0.3;
	double r = 0.05;
	double delta = time / 12.0; //monthly

	vector<double> eurOption;
	vector<double> amerOption;
	for (double s0 = 80; s0 <= 120; s0 += 4)
	{
		eurOption.push_back(Partc(r, delta, sigma, 12, s0, k, 'p'));
		amerOption.push_back(PartcAmer(r, delta, sigma, 12, s0, k, 'p'));
	}

	vector<vector<double>> answers;
	answers.push_back(eurOption);
	answers.push_back(amerOption);
	writeToExcel("Q4.csv", answers);

	//Convergence
	cout << endl << "Prices of european call and american call are as below" << endl;
	writeToConsole(answers);

}

double PartcAmer(double r, double delta, double sigma, int noOfSteps, double s0, double k, char optionType)
{
	double d = exp((r - (pow(sigma, 2) / 2))*delta - sigma*sqrt(delta));
	double u = exp((r - (pow(sigma, 2) / 2))*delta + sigma*sqrt(delta));
	double prob = 0.5;

	//Initialize array
	double** stockPrices = new double*[noOfSteps + 1];
	double** putPrices = new double*[noOfSteps + 1];
	for (int count = 0; count <= noOfSteps; count++)
	{
		stockPrices[count] = new double[noOfSteps + 1];
		putPrices[count] = new double[noOfSteps + 1];
	}

	//Populate stockprices
	stockPrices[noOfSteps][0] = s0;
	for (int count = 1; count <= noOfSteps; count++)
	{
		for (int j = 1; j <= count; j++)
		{
			stockPrices[noOfSteps - j][count] = stockPrices[noOfSteps - j + 1][count - 1] * u;
		}
		stockPrices[noOfSteps][count] = stockPrices[noOfSteps][count - 1] * d;
	}

	//Populate call prices
	for (int count = noOfSteps; count >= 0; count--)
	{
		for (int j = noOfSteps; j >= noOfSteps - count; j--)
		{
			double putPriceCurrent = optionType == 'c' ? max(stockPrices[j][count] - k, 0.0) :
														 max(k - stockPrices[j][count], 0.0);
			double futurePrice = 0.0;
			if (count != noOfSteps) 
			{ 
				futurePrice = (prob * putPrices[j - 1][count + 1] + (1 - prob) * putPrices[j][count + 1]) * exp(-1.0 * r * delta);
 			}
			putPrices[j][count] = futurePrice >= putPriceCurrent ? futurePrice : putPriceCurrent;
		}
	}

	double ret = putPrices[noOfSteps][0];

	//Free up array space
	for (int count = 0; count < noOfSteps + 1; count++)
	{
		delete[] stockPrices[count];
		delete[] putPrices[count];
	}
	delete[] stockPrices;
	delete[] putPrices;
	//////////

	return ret;
}