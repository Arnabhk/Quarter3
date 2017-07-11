#include "Question1.h"

double Question1(double seed)
{
	int n = 1000;
	double a = -0.7;

	double mu[2]{ 0,0 };
	double sd[2]{ 1,1 };
	double cor = a;

	vector<double> x1;
	vector<double> x2;
	GenerateBiVariateNormalDistn(n, seed , mu, sd, cor, x1, x2);

	//Calculate rho(a)
	double x1bar = mean(x1);
	double x2bar = mean(x2);
	double numtr = 0.0, dentr1 = 0.0, dentr2 = 0.0;
	for (int count = 0; count < x1.size(); count++) {
		numtr = numtr + (x1[count] - x1bar)*(x2[count] - x2bar);
		dentr1 = dentr1 + pow(x1[count] - x1bar, 2);
		dentr2 = dentr2 + pow(x2[count] - x2bar, 2);
	}
	//The (n-1) gets cancelled off
	double rhoa = numtr / (sqrt(dentr1) * sqrt(dentr2));
	cout << "The rho calculated using simulation is " << rhoa << endl;
	return rhoa;
}

//Generate BiVariate Normal Distribution
void GenerateBiVariateNormalDistn(int n, double seed, double mu[], double sd[], double cor, vector<double> &x1, vector<double> &x2)
{
	//Generate Normal distribution and make sure the bi-variants are correlated 
	vector<double> z = GenerateNormalDistn(2 * n, seed);
	vector<double> z1(z.begin(), z.begin() + n);
	vector<double> z2(z.begin() + n, z.begin() + 2 * n);

	for (double z1Val : z1) 
	{
		x1.push_back(mu[0] + z1Val*sd[0]);
	}

	double a = sd[1] * cor;
	double b = sd[1] * sqrt(1 - pow(cor,2));
	for (double count = 0; count < z2.size(); count++)
	{
		x2.push_back(mu[1] + a * z1[count] + b * z2[count]);
	}

}