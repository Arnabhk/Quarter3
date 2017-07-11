//To get PI
#define _USE_MATH_DEFINES
#include <cmath>
#include "Question5.h"
#include "Question1.h"
#include <math.h>
#include <time.h>	

using namespace std;

void Question5(double seed)
{
	int N = 5000;
	float* timings = new float[2]{ 0.0,0.0 };
	
	while(abs(timings[1]-timings[0]) <= 0.1)
	{
		timings = GetNormalDistWithStats(N, seed, N == 5000);
		cout << "Timing difference between Polar Marsaglia and Box Muller in seconds for " <<
			N << " normal random numbers is " << timings[1] - timings[0]<<endl;
		N = 2 * N;
	}

	cout << endl << "It can be clearly seen that the Box muller method is more efficient as the number of random numbers to be generated increases"<<endl;

}

float* GetNormalDistWithStats(int N, double seed, bool printStats)
{
	clock_t t1_bm = clock();
	vector<double> normal_boxMuller = GenerateNormalDistn_BoxMuller(N, seed);
	clock_t t2_bm = clock();
	float time_bm = ((float)t2_bm - (float)t1_bm) / CLOCKS_PER_SEC;

	if (printStats)
	{
		double mean_bm = mean(normal_boxMuller);
		double sd_bm = stddev(normal_boxMuller);
		cout << endl << "QUESTION 5" << endl;
		cout << "The mean of box muller normal distribution is " << mean_bm << endl;
		cout << "The standard deviation of box muller normal distribution is " << sd_bm << endl;
	}

	clock_t t1_pm = clock();
	vector<double> normal_polGlia = GenerateNormalDistn_PolarMarsaglia(N, seed);
	clock_t t2_pm = clock();
	float time_pm = ((float)t2_pm - (float)t1_pm) / CLOCKS_PER_SEC;

	if (printStats) 
	{
		double mean_pm = mean(normal_polGlia);
		double sd_pm = stddev(normal_polGlia);
		cout << "The mean of polar marsaglia normal distribution is " << mean_pm << endl;
		cout << "The standard deviation of polar marsaglia normal distribution is " << sd_pm << endl<<endl;
	}

	return new float[2]{ time_bm,time_pm };
}

vector<double> GenerateNormalDistn_BoxMuller(int N, double seed)
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

vector<double> GenerateNormalDistn_PolarMarsaglia(int N, double seed)
{
	vector<double> normalDist;
	vector<double> randNums{ seed };

	int count = 0;
	while (count < N/2)
	{
		randNums = GenerateUniformDist_UsingPrev(2, randNums[randNums.size() - 1]);
		double v1 = 2 * randNums[0] - 1;
		double v2 = 2 * randNums[1] - 1;
		double w = pow(v1, 2) + pow(v2, 2);
		if (w > 1) continue;

		double z1 = v1 * sqrt(-2 * log(w) / w);
		double z2 = v2 * sqrt(-2 * log(w) / w);
		normalDist.push_back(z1);
		normalDist.push_back(z2);
		count++;
	}
	return normalDist;
}

