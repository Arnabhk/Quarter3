#include "Question3.h"
using namespace std;

vector<double> Question3(double seed)
{
	vector<double> answer;
	
	int n = 5000;
	vector<double> z = GenerateNormalDistn(n, seed);

	vector<double> val1 = MonteCarloQ3_1(z, n);
	double val1_mean = mean(val1);
	double val1_std = stddev(val1);
	cout << "a)" << endl;
	cout << "Expected value of a1 = " << val1_mean <<endl;
	cout << "Std Dev of a1 = " << val1_std << endl;

	//Var Reduction a
	double gamma = 1.0;
	vector<double> val1_varRed = MonteCarloQ3_1_VarRed(z, n, val1, gamma);
	double val1_valRed_mean = mean(val1_varRed);
	double val1_valRed_std = stddev(val1_varRed);

	double t[3]{ 0.5,3.2,6.5 };
	//b with var Redn
	vector<double> val2,val2_valRed;
	vector<double> means, stds, means_varRed, stds_varRed;
	for (int i = 0; i < size(t); i++)
	{
		val2 = MonteCarloQ3_2(z, n, t[i]);
		means.push_back(mean(val2));
		stds.push_back(stddev(val2));

		vector<double> val2_varRed = MonteCarloQ3_2_VarRed(z, t[i], n, val2, gamma);
		means_varRed.push_back(mean(val2_varRed));
		stds_varRed.push_back(stddev(val2_varRed));
	}
	
	for (int count = 0; count < means.size(); count++) 
	{
		cout << endl << "Expected value of a" << count + 2 << " = " << means[count] << endl;
		cout << "Standard deviation of a" << count + 2 << " = " << stds[count] << endl;
	}
	
	cout << endl << "b)" << endl;
	cout << "All the values tend towards 1 as we increase the number of simulations." << endl;
	cout << "This matches the theoretical expectation of 1, as E(cos(W_t)) = exp^(-t/2)" << endl;
	cout << endl << "c)" << endl;
	cout << "Variance Reduction :"<<endl<<"For all 4, we can perform a control variate variance reduction strategy" << endl;
	cout << "This strategy involves " << endl <<endl;
	cout << "1) Finding a Y, which has positive correlation with X (original value)" << endl;
	cout << "2) Generating a T = X - gamma * (X - E(X)), where gamma is a constant" << endl;
	cout << "3) Make sure that Var(T) < Var(X)" << endl;

	cout << endl<< "Expected value of a1 with variance reduction = " << val1_valRed_mean << endl;
	cout << "Std Dev of a1 with variance reduction = " << val1_valRed_std << endl;

	for (int count = 0; count < means.size(); count++)
	{
		cout << endl << "Expected value of a" << count + 2 << " with variance reduction = " << 
			means_varRed[count] << endl;
		cout << "Standard deviation of a" << count + 2 << " with variance reduction = " << 
			stds_varRed[count] << endl;
	}
	
	cout << endl << " As can be seen, the standard deviation reduces for all the 4 calculations using the control variate variance reduction technique"<<endl;
	answer.push_back(val1_mean);

	for_each(means.begin(), means.end(), [&](double &n) {answer.push_back(n); });
	answer.push_back(val1_valRed_mean);
	for_each(means_varRed.begin(), means_varRed.end(), [&](double &n) {answer.push_back(n); });

	return answer;
}

vector<double> MonteCarloQ3_1(vector<double> z, int NoOfSim)
{
	vector<double> values;
	for(int count = 0; count < NoOfSim; count++)
	{
		values.push_back(5 * pow(z[count], 2) + sin(sqrt(5)*z[count]));
	}

	return(values);
}

vector<double> MonteCarloQ3_2(vector<double> z, int NoOfSim, double t)
{
	vector<double> values;
	for (int count = 0; count < NoOfSim; count++)
	{
		values.push_back(exp(t/2.0) * cos(sqrt(t)*z[count]));
	}
	return values;
}

vector<double> MonteCarloQ3_1_VarRed(vector<double> z, int NoOfSim, vector<double> values, double gamma)
{
	vector<double> values_valRed;
	vector<double> values_final;

	for (int count = 0; count < NoOfSim; count++)
	{
		values_valRed.push_back(5 * pow(z[count], 2));
	}

	double valRed_mean = 5;
	for (int count = 0; count < NoOfSim; count++)
	{
		double val = values[count] - gamma * (values_valRed[count] - valRed_mean);
		values_final.push_back(val);
	}
	
	return(values_final);
}

vector<double> MonteCarloQ3_2_VarRed(vector<double> z, double t, int NoOfSim, vector<double> values, double gamma)
{
	vector<double> values_valRed;
	vector<double> values_final;

	for (int count = 0; count < NoOfSim; count++)
	{
		values_valRed.push_back(cos(z[count] * sqrt(t)));
	}

	double valRed_mean = exp(-t/2.0);
	for (int count = 0; count < NoOfSim; count++)
	{
		double val = values[count] - gamma * (values_valRed[count] - valRed_mean);
		values_final.push_back(val);
	}

	return(values_final);
}



