#include "Question2.h"
#include "Question3.h"
#include <numeric>
#include <algorithm>

using namespace std;
void Question3(int n, double prob, int total, double seed)
{
	vector<int> binDist = BuildBinomialDist(n, prob, total, seed);
	
	//Plot Histogram
	PlotHistogram(binDist,true);

	//Calculate probability
	int valCheck = 40;
	int validCount = count_if(binDist.begin(), binDist.end(),
		[&](int num) { return num >= valCheck; });
	double probCalc = validCount / binDist.size();
	cout << endl << "QUESTION 3" << endl;
	cout << "Probability that binomial value is atleast 40 is " << probCalc<<endl;

	//Real Value
	cout << "The mathematical formula which needs to be used is P(X>=40,p) = P(X=40) + P(X=41) + P(X=42) + P(X=43) + P(X=44)," << endl;
	cout << "where P(X=k,p) = 44Ck (p^k) ((1-p)^(1-k))" <<endl;
	double probReal = RealBnlProb(44, 40, prob) + RealBnlProb(44, 41, prob) +
		RealBnlProb(44, 42, prob) + RealBnlProb(44, 43, prob) +
		RealBnlProb(44, 44, prob);
	cout << "Probability using math formula that binomial value is atleast 40 is " << probReal<<endl;

	cout << endl << "The probability from mathematical formula is very small and is almost equal to 0, which is the value from the statistical method." << endl;
	cout << endl << "This is because to get 40 true values out of 44 independent observations is very low, i.e. (0.64)^40 ~ 0" << endl;
}

vector<int> BuildBinomialDist(int n, double prob, int total,double seed)
{
	vector<int> binDist;
	for (int count = 0; count < total; count++) 
	{
		double probs[1] = { prob };
		double probVal[1] = { 1 };
 		vector<int> randNum = BuildBernoulliDist(probs,probVal,n,seed);
		binDist.push_back(accumulate(randNum.begin(),randNum.end(),0));
		seed += 10000;
	}

	return binDist;
}

//use formula prob(x = k, p) = nChoosek(p^k) ((1 - p) ^ (n - k))
double RealBnlProb(unsigned n, unsigned k, double prob)
{
	return nChoosek(n, k)*pow(prob, k)*pow(1 - prob, n - k);
}

unsigned nChoosek(unsigned n, unsigned k)
{
	if (k > n) return 0;
	if (n < k * 2) k = n - k;
	if (k == 0) return 1;

	int cmbn = n;
	for (int i = 2; i <= k; ++i) {
		cmbn *= (n - i + 1);
		cmbn /= i;
	}
	return cmbn;
}