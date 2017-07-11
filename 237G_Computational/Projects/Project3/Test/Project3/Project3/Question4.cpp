#include "Question4.h"
#include <algorithm>
using namespace std;

vector<double> Question4(double seed1, double seed2)
{
	double a = 5.8;
	double b = 0.0625;
	double sig = 0.42;
	double v0 = 0.05;
	double s0 = 48;
	double r = 0.03;
	double rho = -0.6;

	double stepSize = 0.01;
	int NoOfSim = 1000;
	double mu[2]{ 0.0,0.0 };
	double sigma[2]{ 1, 1};
	double time = 2.0;
	double K = 30;

	vector<double> reflectionPrice, partialPrice, fullPrice;

	for (int count = 0; count < NoOfSim; count++)
	{
		vector<double> randNums_1, randNums_2;
		GenerateBiVariateNormalDistn(time/stepSize, (seed1*(count+1))+1234, mu, sigma, rho, randNums_1, randNums_2);

		//Reflection
		vector<double> vol_ref = ConstructVolPath(a, b, sig, time, randNums_2, v0, "Reflection");
		vector<double> stocks_ref = ConstructStockPath(r, time, vol_ref, randNums_1, s0, "Reflection");
		reflectionPrice.push_back(exp(-1.0 * r * time) * optionPayoff(stocks_ref[stocks_ref.size() - 1], K));

		//Partial Truncation
		vector<double> vol_pt = ConstructVolPath(a, b, sig, time, randNums_2, v0, "Partial");
		vector<double> stocks_pt = ConstructStockPath(r, time, vol_pt, randNums_1, s0, "Partial");
		partialPrice.push_back(exp(-1.0 * r * time) * optionPayoff(stocks_pt[stocks_pt.size() - 1], K));
		
		//Full Truncation
		vector<double> vol_ft = ConstructVolPath(a, b, sig, time, randNums_2, v0, "Full");
		vector<double> stocks_ft = ConstructStockPath(r, time, vol_ft, randNums_1, s0, "Full");
		fullPrice.push_back(exp(-1.0 * r * time) * optionPayoff(stocks_ft[stocks_ft.size() - 1], K));
	}

	cout << endl << "The mean reflection based price of the European Option = " << mean(reflectionPrice);
	cout << endl << "The mean partial truncation based price of the European Option = " << mean(partialPrice);
	cout << endl << "The mean full truncation based price of the European Option = " << mean(fullPrice)<<endl;

	vector<double> answers;
	answers.push_back(mean(reflectionPrice));
	answers.push_back(mean(partialPrice));
	answers.push_back(mean(fullPrice));
	return(answers);
}

double optionPayoff(double s, double k)
{
	return s > k ? (s - k) : 0;
}


vector<double> ConstructStockPath(double r, double time, vector<double> volatility, vector<double> randNums, double S0, char* method)
{
	double stepSize = 0.01;
	int stepCount = floor(time / stepSize);

	vector<double> values;
	values.push_back(S0);
	for (int count = 1; count <= stepCount; count++)
	{
		double dS;
		if (method == "Reflection")
		{
			dS = r * values[count - 1] * stepSize + sqrt(abs(volatility[count - 1])) * values[count - 1] * randNums[count - 1] * sqrt(stepSize);
		}
		else if (method == "Partial")
		{
			dS = r * values[count - 1] * stepSize + sqrt(max(volatility[count - 1],0.0)) * values[count - 1] * randNums[count - 1] * sqrt(stepSize);
		}
		else if (method == "Full")
		{
			dS = r * values[count - 1] * stepSize + sqrt(max(volatility[count - 1],0.0)) * values[count - 1] * randNums[count - 1] * sqrt(stepSize);
		}
		values.push_back(values[count - 1] + dS);
	}

	return values;
}



vector<double> ConstructVolPath(double a, double b, double sig, double time, vector<double> randNums, double V0, char* method)
{
	double stepSize = 0.01;
	int stepCount = floor(time / stepSize);

	vector<double> values;
	values.push_back(V0);
	for (int count = 1; count <= stepCount; count++)
	{
		double dV;
		double VNew;
		if (method == "Reflection")
		{
			dV = a * (b - abs(values[count - 1])) * stepSize + (sig * sqrt(abs(values[count - 1])) * randNums[count - 1] * sqrt(stepSize));
			VNew = abs(values[count - 1]) + dV;
		}
		else if (method == "Partial")
		{
			dV = a * (b - values[count - 1]) * stepSize + (sig * sqrt(max(values[count - 1],0.0)) * randNums[count - 1] * sqrt(stepSize));
			VNew = values[count - 1] + dV;
		}
		if (method == "Full")
		{
			dV = a * (b - max(values[count - 1],0.0)) * stepSize + (sig * sqrt(max(values[count - 1],0.0)) * randNums[count - 1] * sqrt(stepSize));
			VNew = values[count - 1] + dV;
		}
		values.push_back(VNew);
	}
	
	return values;
}