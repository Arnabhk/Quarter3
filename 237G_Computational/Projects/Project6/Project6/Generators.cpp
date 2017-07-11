#include "Generators.h"
#include <random>
#include<algorithm>
using namespace std;

vector<double> GenerateFullStockPath(double T, double S0, double r, double sigma, double stepSize, double seed, bool logPrices)
{
	vector<double> stockPrices;
	double NoOfSteps = floor(T / stepSize);
	stockPrices.push_back(logPrices?log(S0):S0);
	vector<double> randNums = GenerateNormalDistn(NoOfSteps, seed);
	for (int count = 1; count <= NoOfSteps; count++)
	{
		double St = logPrices? stockPrices[count - 1] + (r - pow(sigma, 2) / 2)*stepSize + sigma*sqrt(stepSize)*randNums[count - 1]:
							   stockPrices[count - 1] * exp((r - pow(sigma, 2) / 2)*stepSize + sigma*sqrt(stepSize)*randNums[count - 1]);
		stockPrices.push_back(St);
	}
	return stockPrices;
}
vector<double> GenerateNormalDistn(int N, double seed)
{
	vector<double> normalDist;

	std::default_random_engine generator(seed);
	std::normal_distribution<double> distribution;
	for (int i = 0; i < N; i++)
	{
		normalDist.push_back(distribution(generator));
	}
	return normalDist;
}
vector<double> GenerateExpDistn(int N, double seed, double lambda)
{
	vector<double> finalVals;

	vector<double> randNums = GenerateUniformDist(N, seed);
	for (int count = 0; count < randNums.size(); count++)
	{
		finalVals.push_back(-1.0 * lambda * log(1 - randNums[count]));
	}

	return finalVals;
}
vector<double> GenerateUniformDist(int N, double seed)
{
	vector<double> uniformDist;

	default_random_engine generator(seed);
	uniform_real_distribution<double> distribution(0.0, 1.0);
	for (int i = 0; i < N; i++)
	{
		uniformDist.push_back(distribution(generator));
	}
	return uniformDist;
}
vector<double> GeneratePoissonDist(int N, double gamma, double stepSize, double seed)
{
	vector<double> poissonDist;
	vector<double> randNums = GenerateUniformDist(N, seed);
	poissonDist.push_back(0);
	for (int count = 1; count < N; count++)
	{
		double z = exp(-1.0 * gamma*stepSize);
		double x = z;
		double randNum = randNums[count];
		int k = 0;
		while (randNum >= x)
		{
			z = z * gamma * stepSize / (k + 1);
			x = x + z;
			k++;
		}
		poissonDist.push_back(k);
	}

	return poissonDist;
}
