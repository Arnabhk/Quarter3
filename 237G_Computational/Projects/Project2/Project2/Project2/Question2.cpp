#include "Question2.h"

using namespace std;
double Question2(double seed)
{
	vector<double> x1;
	vector<double> x2;

	int n = 5000;
	double mu[2]{ 0,0 };
	double sd[2]{ 1,1 };
	double cor = 0.6;
	GenerateBiVariateNormalDistn(n, seed, mu, sd, cor, x1, x2);
	
	double expected_val = MonteCarloQ2(x1, x2, n);
	cout << "The expected value using simulation is " << expected_val << endl;
	return(expected_val);
}

double MonteCarloQ2(vector<double> x, vector<double> y, int NoOfSim)
{
	vector<double> values;
	for (int count = 0; count < NoOfSim; count++)
	{
		double val1 = pow(x[count], 3);
		double val2 = sin(y[count]);
		double val3 = pow(x[count], 2)*y[count];

		values.push_back(val1 + val2 + val3 > 0 ? val1 + val2 + val3 : 0);
	}

	return mean(values);
}

