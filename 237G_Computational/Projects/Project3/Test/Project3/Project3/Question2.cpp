#include "Question2.h"
#include <algorithm>
using namespace std;

vector<double> Question2(double seed1, double seed2)
{
	double X0 = 1;
	double Y0 = 1;
	
	int NoOfSim = 1000;
	double stepSize = 0.01;

	vector<double> y3;
	for (int count = 0; count < NoOfSim; count++)
	{
		y3.push_back(GenerateYPath(Y0, stepSize, 3, (int)(seed1*count) / 1000, (int)(seed2*count) / 1000)[floor(3 / stepSize)]);
	}

	vector<double> x3;
	for (int count = 0; count < NoOfSim; count++)
	{
		x3.push_back(GenerateXPath(X0, stepSize, 3, (int)(seed1*count) / 1000, (int)(seed2*count) / 1000)[floor(3 / stepSize)]);
	}

	for_each(x3.begin(), x3.end(), [](double& num) {num = cbrt(1 + num); });
	for_each(y3.begin(), y3.end(), [](double& num) {num = cbrt(1 + num); });

	double expa = mean(x3);
	double expb = mean(y3);

	cout << endl << "Expected value of (1+X3)^(1/3) = " << expa << endl;
	cout << "Expected value of (1+Y3)^(1/3) = " << expb << endl;

	vector<double> answers{ expa,expb };
	return answers;
}

vector<double> GenerateXPath(double X0, double stepSize, double Time, double seed1, double seed2)
{
	vector<double> pathValues;
	pathValues.push_back(X0);
	int NoOfSteps = floor(Time / stepSize);

	vector<double> randNums_1 = GenerateNormalDistn(NoOfSteps, seed1);
	vector<double> randNums_2 = GenerateNormalDistn(NoOfSteps, seed2);

	for (int count = 1; count <= NoOfSteps; count++)
	{
		double term1 = 0.25 * pathValues[count - 1] * stepSize;
		double term2 = (1.0 / 3.0) * pathValues[count - 1] * randNums_1[count - 1] * sqrt(stepSize);
		double term3 = (3.0 / 4.0) * pathValues[count - 1] * randNums_2[count - 1] * sqrt(stepSize);
		double Xt = pathValues[count - 1] + term1 + term2 - term3;
		pathValues.push_back(Xt);
	}
	return pathValues;
}

vector<double> GenerateYPath(double Y0, double stepSize, double Time, double seed1, double seed2)
{
	vector<double> pathValues;
	pathValues.push_back(Y0);
	int NoOfSteps = floor(Time / stepSize);

	vector<double> randNums_1 = GenerateNormalDistn(NoOfSteps, seed1);
	vector<double> randNums_2 = GenerateNormalDistn(NoOfSteps, seed2);

	for (int count = 1; count <= NoOfSteps; count++)
	{
		double val = -0.08*stepSize + (1.0 / 3.0)*randNums_1[count - 1] * sqrt(stepSize) + (3.0 / 4.0)*randNums_2[count - 1] * sqrt(stepSize);
		pathValues.push_back(exp(val));
	}
	return pathValues;
}