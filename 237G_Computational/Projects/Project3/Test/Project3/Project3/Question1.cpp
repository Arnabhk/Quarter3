#include "Question1.h"
#include <algorithm>

using namespace std;

vector<double> Question1(double seed1, double seed2)
{
	double X0 = 1;
	double Y0 = 0.75;

	int NoOfSim = 1000;
	double stepSize = 0.01;

	//Part a
	vector<double> y2;
	for (int count = 0; count < NoOfSim; count++)
	{
		y2.push_back(GenerateYPath(Y0, stepSize, 2, (int)(seed2*count)/1000)[floor(2/stepSize)]);
	}
	double proba = count_if(y2.begin(), y2.end(), [](double num) { return num > 5; })/(double)NoOfSim;

	//Part b
	vector<double> x2;
	for (int count = 0; count < NoOfSim; count++)
	{
		x2.push_back(GenerateXPath(X0, stepSize, 2, (int)(seed1*count)/1000)[floor(2 / stepSize)]);
	}

	vector<double> x2_transform(x2.size());
	copy(x2.begin(), x2.end(), x2_transform.begin());
	for_each(x2_transform.begin(), x2_transform.end(), [](double& num) {num = cbrt(num); });

	double expb = mean(x2_transform);

	//Part c
	vector<double> y3;
	for (int count = 0; count < NoOfSim; count++)
	{
		y3.push_back(GenerateYPath(Y0, stepSize, 3, seed2*count)[floor(3 / stepSize)]);
	}
	double expc = mean(y3);

	//Part d
	vector<double> x2y2;
	for (int count = 0; count < NoOfSim; count++)
	{
		if(x2[count] > 1)
		{
			x2y2.push_back(x2[count] * y2[count]);
		}
		else
		{
			x2y2.push_back(0);
		}
		
	}
	double expd = mean(x2y2);


	cout << endl << "Prob(Y2 > 5) = " << proba << endl;
	cout << "Expected Value of X2 ^ (1/3) = " << expb << endl;
	cout << "Expected Value of Y3 = " << expc << endl;
	cout << "Expected Value of X2Y2 1(X2>1) = " << expd << endl;

	vector<double> answers{ proba,expb,expc,expd };
	return(answers);
}

//Generate the X Path
vector<double> GenerateXPath(double X0, double stepSize, double Time, double seed)
{
	vector<double> pathValues;
	pathValues.push_back(X0);
	int NoOfSteps = floor(Time / stepSize);

	vector<double> randNums = GenerateNormalDistn(NoOfSteps, seed);
	for (int count = 1; count <= NoOfSteps; count++)
	{
		double Xt = pathValues[count - 1] + (0.2 - 0.5 * pathValues[count - 1])*stepSize + (2.0 / 3.0)*randNums[count - 1] * sqrt(stepSize);
		pathValues.push_back(Xt);
	}
	return pathValues;
}

//Generate the Y Path
vector<double> GenerateYPath(double Y0, double stepSize, double Time, double seed)
{
	vector<double> pathValues;
	pathValues.push_back(Y0);
	int NoOfSteps = floor(Time / stepSize);

	vector<double> randNums = GenerateNormalDistn(NoOfSteps, seed);
	for (int count = 1; count <= NoOfSteps; count++)
	{
		double t = (count - 1) * stepSize;
		double dt_term = (2.0 / (1 + t))*pathValues[count - 1] + ((1 + pow(t,3)) / 3);
		double Wt_term = ((1 + pow(t, 3)) / 3);

		double Yt = pathValues[count - 1] + dt_term * stepSize + Wt_term * randNums[count - 1] * sqrt(stepSize);
		pathValues.push_back(Yt);
	}
	return pathValues;
}