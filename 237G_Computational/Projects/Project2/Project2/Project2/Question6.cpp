#include "Question6.h"
#include <numeric>

vector<double> Question6(double seed)
{
	vector<double> answers;

	int a = 0;
	int b = 1;
	double h = (double)1.0 / 10000.0;
	double euler_val = EulerDiscretization(h, a, b);
	answers.push_back(euler_val);

	int NoOfSim = 10000;
	vector<double> vals1 = MonteCarloIntegration(NoOfSim, seed);
	vector<double> vals2 = ImportanceSampling(NoOfSim, seed);

	double mc_Mean = mean(vals1);
	answers.push_back(mc_Mean);
	double mc_Std = stddev(vals1);
	
	cout << "The value of pi from Euler Discretization is " << euler_val << endl;
	cout << "The expected value of pi from Monte Carlo Simulation is " << mc_Mean << endl;
	double is_Mean = mean(vals2);
	double is_Std = stddev(vals2);
	cout << "The expected value of pi from Importance Sampling is " << is_Mean<< endl;

	cout << endl<< "The standard deviation of pi from Monte Carlo Simulation is " << mc_Std << endl;
	cout << "The standard deviation of pi from Importance Sampling is " << is_Std << endl;
	cout << "There is a reduction in standard deviation due to Importance Sampling";
	answers.push_back(is_Mean);

	return(answers);

}

double EulerDiscretization(double h,int a, int b)
{
	int NoOfSteps = (b - a) / h;
	vector<double> stepVals;
	stepVals.push_back(sqrt(1 - pow(a,2)));
	for(int count = 1; count<=(NoOfSteps-1); count++)
	{
		double x = a + count*h;
		stepVals.push_back(stepVals[count - 1] + (h * ((-1.0 * x)/sqrt(1 - pow(x, 2)))));
	}
	stepVals.push_back(sqrt(1 - pow(b, 2)));
	return 4 * accumulate(stepVals.begin(),stepVals.end(),0.0) * h;
}

vector<double> MonteCarloIntegration(int NoOfSim, double seed)
{
	vector<double> uniVals = GenerateUniformDist(NoOfSim, seed);
	vector<double> results;
	for (int count = 0; count < NoOfSim; count++)
	{
		double yVal = sqrt(1 - pow(uniVals[count], 2));
		results.push_back(yVal*4.0);
	}
	return(results);
}

vector<double> ImportanceSampling(int NoOfSim, double seed)
{
	double a = 0.74;
	vector<double> uniVals = GenerateUniformDist(NoOfSim, seed);

	vector<double> results;
	for (int count = 0; count < NoOfSim; count++)
	{
		//Acceptance Rejection method
		//check if x <= t(x)/f(x) where t(x) and f(x) are pdfs
		// in this case f(x) = 1
		double tx = (1 - a * pow(uniVals[count], 2)) / (1 - a / 3.0);
		if (uniVals[count] > tx)
		{
			continue;
		}
		
		double gx = sqrt(1 - pow(uniVals[count], 2));
		results.push_back(gx*4.0/tx);
	}
	return(results);
}
