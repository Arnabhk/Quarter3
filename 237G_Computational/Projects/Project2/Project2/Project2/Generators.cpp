#define _USE_MATH_DEFINES
#include <cmath>
#include "Generators.h"	
using namespace std;

vector<double> GenerateNormalDistn(int N, double seed)
{
	vector<double> randNums = GenerateUniformDist(N, seed);
	vector<double> randNums_1(randNums.begin(), randNums.begin() + (N / 2));
	vector<double> randNums_2(randNums.begin() + (N / 2), randNums.begin() + N);
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

vector<double> GenerateUniformDist(int N, double seed)
{
	const unsigned long long m = pow(2, 31) - 1;
	const long a = pow(7, 5);
	const int b = 0;

	//1a - random number generator
	vector<double> randNum;
	//add seed. will remove it later
	randNum.push_back(seed);
	for (int count = 1; count <= N; count++)
	{
		unsigned long long temp = a*(randNum[count - 1]) + b;
		randNum.push_back(temp % m);
	}
	randNum.erase(randNum.begin());
	//Convert to U(0,1)
	for (int count = 0; count < N; count++)
	{
		randNum[count] = (randNum[count] + 0.5) / m;
	}

	return randNum;
}

vector<double> GenerateStockPath(double T, double S0, double r, double sigma, int NoOfPaths, vector<double> randNums)
{
	vector<double> stockPrices;
	
	for (int count = 0; count < NoOfPaths; count++)
	{
		double St = S0 * exp((r - pow(sigma, 2) / 2)*T + sigma*sqrt(T)*randNums[count]);
		stockPrices.push_back(St);
	}
	return stockPrices;
}

vector<double> GenerateFullStockPath(double T, double S0, double r, double sigma, double stepSize, double seed)
{
	vector<double> stockPrices;
	double NoOfSteps = floor(T / stepSize);
	stockPrices.push_back(S0);
	vector<double> randNums = GenerateNormalDistn(NoOfSteps, seed);
	for (int count = 1; count <= NoOfSteps; count++)
	{
		double St = stockPrices[count-1]* exp((r - pow(sigma, 2) / 2)*stepSize + sigma*sqrt(stepSize)*randNums[count-1]);
		stockPrices.push_back(St);
	}
	return stockPrices;
}

vector<double> GenerateVarianceReductionDistbn(vector<double> val1, vector<double> val2)
{
	vector<double> val_combined(val1.size());
	transform(val1.begin(), val1.end(), val2.begin(), val_combined.begin(), std::plus<double>());
	for_each(val_combined.begin(), val_combined.end(), [](double& n) {n = n / 2.0; });
	return(val_combined);
}