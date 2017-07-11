#include "Question1.h"
#include "Question4.h"
#include <math.h>
#include <algorithm>
using namespace std;

void Question4(int N, double lambda, double seed)
{
	vector<double> expNums = BuildExponDist(N, lambda, seed);

	//Calculate Probability
	int validCount1 = count_if(expNums.begin(), expNums.end(),
		[&](int num) { return num >= 1; });
	double prob1 = (double)validCount1 / (double)expNums.size();

	int validCount2 = count_if(expNums.begin(), expNums.end(),
		[&](int num) { return num >= 4; });
	double prob2 = (double)validCount2 / (double)expNums.size();

	cout << endl << "QUESTION 4" << endl;
	cout << "Probability that exponential distribution is greater than 1 " << prob1 << endl;
	cout << "Probability that exponential distribution is greater than 4 " << prob2 << endl;

	//Plot Histogram
	PlotHistogram(expNums,false);

	//Mean,Variance
	double mean_exp = mean(expNums);
	double sd_exp = stddev(expNums);
	cout << "Mean of exponential distribution is " << mean_exp << endl;
	cout << "Std Deviation of exponential distribution is " << sd_exp << endl;
}

vector<double> BuildExponDist(int N, double lamda, double seed)
{
	vector<double> finalVals;

	vector<double> randNums = GenerateUniformDist(N, seed);
	for (int count = 0; count < randNums.size(); count++)
	{
		finalVals.push_back(-1.0 * lamda * log(1 - randNums[count]));
	}

	return finalVals;

}
