#define _USE_MATH_DEFINES
#include <cmath>
#include "Generators.h"
#include <random>

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

//Generate BiVariate Normal Distribution
void GenerateBiVariateNormalDistn(int n, double seed, double mu[], double sd[], double cor, vector<double> &x1, vector<double> &x2)
{
	//Generate Normal distribution and make sure the bi-variants are correlated 
	vector<double> z = GenerateNormalDistn(2 * n, seed);
	vector<double> z1(z.begin(), z.begin() + n);
	vector<double> z2(z.begin() + n, z.begin() + 2 * n);

	for (double z1Val : z1)
	{
		x1.push_back(mu[0] + z1Val*sd[0]);
	}

	double a = sd[1] * cor;
	double b = sd[1] * sqrt(1 - pow(cor, 2));
	for (double count = 0; count < z2.size(); count++)
	{
		x2.push_back(mu[1] + a * z1[count] + b * z2[count]);
	}

}

