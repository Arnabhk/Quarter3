#include<algorithm>
#include "Question1.h"
#include "Question2.h"
#include <numeric>
using namespace std;

void Question2(double probs[], double probVals[], int N, double seed)
{
	vector<int> finalVals = BuildBernoulliDist(probs, probVals, N, seed);

	double mean_seq = mean(finalVals);
	double sd_seq = stddev(finalVals);
	cout << endl << "QUESTION 2" << endl;
	cout << "The Mean of Bernoulli is " << mean_seq << endl;
	cout << "The Standard Deviation of Bernoulli is " << sd_seq << endl;

	PlotHistogram(finalVals, true);
}

vector<int> BuildBernoulliDist(double probs[],double probVals[],int N,double seed)
{
	vector<double> randNums = GenerateUniformDist(N,seed);
	
	int size = sizeof(probs);
	vector<int> probCount;
	
	for (int count = 0; count < size; count++)
	{
		double startProb = 0.0, endProb = 0.0;
		for (int ct = 0; ct < count; ct++) { startProb += probs[ct]; }
		for (int ct = 0; ct <= count; ct++) { endProb += probs[ct]; }

		probCount.push_back(count_if(randNums.begin(), randNums.end(),
			[&](double num) { return isInProbRange(num, startProb, endProb); }));
	}

	vector<int> finalVals(randNums.size(),0);
	int valFilled = 0;
	for (int count = 0; count < size; count++)
	{
		fill_n(finalVals.begin() + valFilled, probCount[count], probVals[count]);
		valFilled = valFilled + probCount[count];
	}

	return(finalVals);
}

bool isInProbRange(double num, double startProb, double endProb)
{
	return num > startProb && num <= endProb;
}

