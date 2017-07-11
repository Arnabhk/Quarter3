#include "Question1.h"
#include "Question2.h"
#include <fstream>
#include <algorithm>
#include <string>
#include <map>
#include <limits>
using namespace std;

void Proj6_2func()
{
	double lambda1 = 0.2;
	double lambda2 = 0.4;
	double T = 5;
	vector<double> answers = CalculateDefaultInfo(lambda1, lambda2, T);
	cout << "The default case lambda1=0.2, lambda2=0.4, T = 5" << endl;
	writeToConsole(answers);

	vector<vector<double>> allAnswers;
	
	lambda1 = 0.2;
	for (lambda2 = 0.0; lambda2 <= 0.8; lambda2 += 0.1)
	{
		for (T = 3; T <= 8; T++)
		{
			cout << lambda1 << " " << lambda2 << " " << T<<endl;
			vector<double> result = CalculateDefaultInfo(lambda1, lambda2, T);
			vector<double> allAnswer{ lambda1,lambda2,T };
			allAnswer.insert(allAnswer.end(), result.begin(), result.end());
			allAnswers.push_back(allAnswer);
		}
	}

	lambda2 = 0.4;
	for (lambda1 = 0.05; lambda1 <= 0.4; lambda1 += 0.05)
	{
		for (T = 3; T <= 8; T++)
		{
			cout << lambda1 << " " << lambda2 << " " << T << endl;
			vector<double> result = CalculateDefaultInfo(lambda1, lambda2, T);
			vector<double> allAnswer{ lambda1,lambda2,T };
			allAnswer.insert(allAnswer.end(), result.begin(), result.end());
			allAnswers.push_back(allAnswer);
		}
	}
	

	writeToExcel("AllAnswers.csv", allAnswers);
}

vector<double> CalculateDefaultInfo(double lambda1, double lambda2, double T)
{
	//Constants
	double seed = 1234;
	double mu = -0.1;
	double sigma = 0.2;
	double gamma = -0.4;
	double v0 = 20000;
	double stepSize = (1.0 / 12.0);
	double NoOfPaths = 10000;
	double r0 = 0.02;
	double delta = 0.25;
	double L0 = 22000;
	double alpha = 0.7;
	double epsilon = 0.95;
	double beta = (epsilon - alpha) / T;

	//Generate loan and qt path beforehand as it is deterministic
	vector<double> loanPath = GenerateLoanPath(L0, r0, delta, lambda2, stepSize, T);
	vector<double> qt = GenerateLoanFrac(alpha, beta, stepSize, T);

	vector<double> prices;
	vector<double> Excercisetime;
	int numOfDefaults = 0;
	for (int count = 0; count < NoOfPaths; count++)
	{
		//vector<double> collPricePath = GenerateCollPricePath_Method1(v0, mu, sigma, gamma, lambda1, stepSize, T, count, seed);
		vector<double> collPricePath = GenerateCollPricePath_Method2(v0, mu, sigma, gamma, lambda1, stepSize, T, count, seed);
		double stoppingQ = CalculateQStopping(collPricePath, loanPath, qt, stepSize);
		double stoppingS = CalculateSStopping(seed, count, lambda2);
		double tau = CalculateTau(stoppingQ, stoppingS, T);

		double payoff = 0;
		//if tau is max num, then both Q and S where not less than T
		if (tau != numeric_limits<double>::max())
		{
			payoff = CalculatePayoff(stoppingQ, stoppingS, tau, stepSize, epsilon, collPricePath, loanPath);
			numOfDefaults++;
			Excercisetime.push_back(tau);
		}
		prices.push_back(payoff * (tau != numeric_limits<double>::max() ? exp(-1.0 * r0 * tau) : 0.0));
	}

	double averagePrice = mean(prices);
	double averageExerTime = mean(Excercisetime);
	double prob = numOfDefaults / NoOfPaths;

	vector<double> answers = { averagePrice, averageExerTime, prob };
	return answers;
}

vector<double> GenerateCollPricePath_Method2(double v0, double mu, double sigma, double gamma, double lambda1, double stepSize, double time, double pathNo, double seed)
{
	vector<double> pricePath;
	vector<double> timePath;
	pricePath.push_back(log(v0));
	int NoOfSteps = floor(time / stepSize);

	//Find taus until totaltime reaches input time
	double totalTime = 0.0;
	bool jumpsPresent = true;
	vector<int> jumpPos(NoOfSteps, 0);
	while (jumpsPresent)
	{
		double tau = GenerateExpDistn(1, seed*(totalTime + 47)*(pathNo + 1) * 10, 1.0 / lambda1)[0];
		totalTime = totalTime + tau;
		if (totalTime >= (time-stepSize)) break;
		jumpPos[ceil(totalTime / stepSize)] = 1;
	}

	//Find stock path using the jumps
	vector<double> randNums = GenerateNormalDistn(NoOfSteps, (seed*(pathNo + 1) * 43));
	for (int count = 0; count < NoOfSteps; count++)
	{
		double dv = (mu - (pow(sigma, 2)) / 2)*stepSize + (sigma * sqrt(stepSize) * randNums[count]) + gamma*jumpPos[count];
		pricePath.push_back(pricePath[count] + dv);
	}
	pricePath.erase(pricePath.begin());
	for_each(pricePath.begin(), pricePath.end(), [](double& stk) {stk = exp(stk); });

	return pricePath;
}

vector<double> GenerateCollPricePath_Method1(double v0, double mu, double sigma, double gamma, double lambda1, double stepSize, double time, double pathNo, double seed)
{
	vector<double> stockPath = GenerateFullStockPath(time, v0, mu, sigma, stepSize, seed*((pathNo + 1) + 2.0), true);
	stockPath.erase(stockPath.begin());

	//Generate poisson distribution with lambda1*stepSize
	vector<double> jumpCount = GeneratePoissonDist(floor(time / stepSize), lambda1, stepSize, seed*(pathNo + 1)*2.0);
	vector<double> jumpVal = cumsum(jumpCount);
	for_each(jumpVal.begin(), jumpVal.end(), [&](double& num) {num = num * gamma; });

	for (int count = 0; count < stockPath.size(); count++)
	{
		stockPath[count] = stockPath[count] + jumpVal[count];
	}

	for_each(stockPath.begin(), stockPath.end(), [](double& stkPath) {stkPath = exp(stkPath); });

	return stockPath;
}

vector<double> GenerateLoanPath(double L0, double r0, double delta, double lambda2, double stepSize, double time)
{
	vector<double> loanPath;

	double R = r0 + delta * lambda2;
	double r = R / 12.0;
	double n = time * 12;

	double pmt = (L0 * r) / (1 - pow(1 / (1 + r), n));
	double a = pmt / r;
	double b = pmt / (r * pow(1 + r, n));
	double c = 1 + r;

	int noOfSteps = floor(time / stepSize);
	for (int count = 0; count < noOfSteps; count++)
	{
		double loanAmt = a - (b * pow(c, 12 * (count + 1)*stepSize));
		loanPath.push_back(loanAmt);
	}

	return loanPath;
}

vector<double> GenerateLoanFrac(double alpha, double beta, double stepSize, double time)
{
	vector<double> qt;
	int noOfSteps = floor(time / stepSize);
	for (int count = 0; count < noOfSteps; count++)
	{
		qt.push_back(alpha + (beta * (count + 1) * stepSize));
	}
	return qt;
}

double CalculateQStopping(vector<double> collPricePath, vector<double> loanPath, vector<double> qt, double stepSize)
{
	//Find the first step where collateral price is less than q*loan price
	for (int count = 0; count < collPricePath.size(); count++)
	{
		if (collPricePath[count] <= qt[count] * loanPath[count])
		{
			return (count + 1)*stepSize;
		}
	}
	return numeric_limits<double>::max();

}

double CalculateSStopping(double seed, int pathCount, double lambda2)
{
	return GenerateExpDistn(1, (seed*(pathCount + 1.0) * 37), 1.0 / lambda2)[0];
}

double CalculateTau(double stoppingQ, double stoppingS, double time)
{
	if (stoppingS >= time) { stoppingS = numeric_limits<double>::max(); }
	return min(stoppingQ, stoppingS);
}

double CalculatePayoff(double stoppingQ, double stoppingS, double tau, double stepSize, double epsilon, vector<double> collPricePath, vector<double> loanPath)
{
	int pos = (tau == stoppingQ) ? (stoppingQ / stepSize)-1 : floor(stoppingS / stepSize);
	double payoff = (tau == stoppingQ) ? max(loanPath[pos] - epsilon*collPricePath[pos], 0.0)
		: abs(loanPath[pos] - epsilon*collPricePath[pos]);
	return payoff;
}