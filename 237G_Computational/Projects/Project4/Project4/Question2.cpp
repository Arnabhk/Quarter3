#include "Question1.h"
#include "Question2.h"
#include <fstream>
#include <algorithm>
#include <string>
#include <map>
using namespace std;

void Question2()
{
	double r = 0.02;
	vector<double> prices;
	vector<int> yearOfPrices;
	//Retrieve yahoo prices
	RetrievePrices(prices, yearOfPrices);
	double s0 = prices[prices.size() - 1];

	//Get closest multiple for strike
	double k = ClosestMultiple(s0);

	//9 months minus 8 days as yahoo finance expiry is 19th Jan 2018
	double time = 0.75 - (8.0/252.0); 

	//Calculate annual returns using prices
	vector<double> returns = CalculateAnnualReturns(prices,yearOfPrices);
	double sigma = stddev(returns);
	cout << "Historical Sigma = " << sigma << endl;
	int N = 5;
	double delta = time / N;
	double optionPrice = Partc(r, delta, sigma, N, s0, k);
	cout << "Option Price with 60 month historical volatility = " << optionPrice<<endl;

	//calculate implied vol 
	double market_call = 23.68;
	double impl_sigma = CalculateImpliedVol(sigma, delta, r, N, s0, k, market_call);
	cout << "Implied Volatility = " << impl_sigma << endl;

	optionPrice = Partc(r, delta, impl_sigma, N, s0, k);
	cout << "Option price using implied volatility = " << optionPrice << endl;
}

void RetrievePrices(vector<double>& prices, vector<int>& years)
{
	ifstream data;
	string date,price;
	data.open("Google_data.csv");
	int count = 0;
	while (data.is_open() && !data.eof())
	{
		getline(data,date,',');
		if (date == "")
		{
			break;
		}
		getline(data, price, '\n');
		if (count == 0)
		{
			count++;
			continue; 
		}
		years.push_back(stoi(date.substr(date.find_last_of('/')+1, 4)));
		prices.push_back(stod(price));
		count++;
	}
}

vector<double> CalculateAnnualReturns(vector<double> prices, vector<int> years)
{
	map<int, double> annual_returns;
	for (int count = 1; count < prices.size(); count++)
	{
		double newRet = (prices[count] - prices[count - 1]) / prices[count - 1];
		double currentVal = annual_returns[years[count]];
		annual_returns[years[count]] = ((1 + currentVal) * (1 + newRet)) - 1;
	}

	vector<double> returns;
	for (auto const & p : annual_returns)
	{
		returns.push_back(p.second);
	}
	return returns;
}

double ClosestMultiple(double x)
{
	int val = (x * 1.1) / 10;
	int smallVal = val * 10;
	int bigVal = (val+1) * 10;
	return val - smallVal <= bigVal - val ? smallVal : bigVal;
}

double CalculateImpliedVol(double sigma, double delta, double r, double N, double s0, double k, double market_call)
{
	bool solved = false;
	double impl_sigma;
	for (double sigma_b = sigma - 0.01; !solved; sigma_b = sigma_b - 0.01)
	{
		double optionPriceb = Partc(r, delta, sigma_b, N, s0, k);
		if (abs(optionPriceb - market_call) <= 0.1)
		{
			impl_sigma = sigma_b;
			solved = true;
		}
	}

	return impl_sigma;
}