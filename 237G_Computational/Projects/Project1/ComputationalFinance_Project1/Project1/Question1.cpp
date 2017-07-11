#include "Question1.h"
#include <random>
using namespace std;

const unsigned long long m = pow(2, 31) - 1;
const long a = pow(7, 5);
const int b = 0;

void Question1(int N, double seed)
{
	vector<double> randNum = GenerateUniformDist(N, seed);

	double mean_lgm = mean(randNum);
	double sd_lgm = stddev(randNum);
	cout << endl << "QUESTION 1" << endl;
	cout << "The Mean of LGM is " << mean_lgm << endl;
	cout << "The Standard Deviation of LGM is " << sd_lgm << endl;

	//1b - Use C++ generator
	default_random_engine generator;
	uniform_real_distribution<double> distribution(0.0, 1.0);
	vector<double> randNum_inbuilt;
	for (int count = 0; count < N; count++)
	{
		randNum_inbuilt.push_back(distribution(generator));
	}
	double mean_inbuilt = mean(randNum_inbuilt);
	double sd_inbuilt = stddev(randNum_inbuilt);
	cout << endl << "The Mean of Inbuilt is " << mean_inbuilt << endl;
	cout << "The Standard Deviation of Inbuilt is " << sd_inbuilt << endl;

	cout << endl << "The mean and standard deviation between the two methods are very similar." << endl;
	cout << "The small difference might be due to the choice of the seed and the default random generator might not be using the same parameters as the LGM method"<<endl;
}

vector<double> GenerateUniformDist(int N,double seed)
{
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

vector<double> GenerateUniformDist_UsingPrev(int N, double prevVal)
{
	double seed = prevVal*m - 0.5;
	return GenerateUniformDist(N, seed);
}

