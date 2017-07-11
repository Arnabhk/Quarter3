#define _USE_MATH_DEFINES
#include <cmath>
#include "Question3.h"
#include "Question1.h"
#include <algorithm>
#include <Windows.h>
using namespace std;

void Question3()
{
	double k = 50;
	double r = 0.03;
	double sigma = 0.2;
	double time = 0.3846;

	//Parta
	int noOfStepsa = 100;
	double deltaa = time / noOfStepsa;
	double su, c0, cu;
	vector<double> answera;
	for (double s0 = 20; s0<=80.0; s0+=2)
	{
		c0 = Partc(r, deltaa, sigma, noOfStepsa, s0, k);
		su = s0 + 0.01;
		cu = Partc(r, deltaa, sigma, noOfStepsa, su, k);
		double deltaVal = (cu - c0) / (su - s0);
		answera.push_back(deltaVal);
	}
	
	//Partb
	double s0 = 49;
	vector<double> answerb;
	double deltaTime = 0.01;
	int noOfStepsb = 1000;
	for (double timeCount = 0; timeCount <= time; timeCount+= deltaTime)
	{
		//Call Part c from previous question for binomial call pricing
		double c0 = Partc(r, timeCount/noOfStepsb, sigma, noOfStepsb, s0, k);
		su = s0 + 0.01;
		double cu = Partc(r, timeCount/noOfStepsb, sigma, noOfStepsb, su, k);
		answerb.push_back((cu - c0) / (su - s0));
	}

	//Part c
	vector<double> answerc;
	double deltac = 0.0005;
	int noOfStepsc = (int)(time / deltac);
	for (double s0 = 20; s0 <= 80; s0 += 2)
	{
		c0 = Partc(r, deltac, sigma, noOfStepsc, s0, k);
		cu = Partc(r, deltac, sigma, noOfStepsc-5, s0, k);
		answerc.push_back((cu - c0) / (5*deltac));
	}

	//Part d
	vector<double> answerd;
	double deltad = 0.001;
	int noOfStepsd = (int)(time / deltad);

	double suu, cuu, deltau, deltauu, gamma;

	//first gamma is calculated for point 1.
	answerd.push_back(0);
	for (double s0 = 20; s0 <= 80; s0 += 2)
	{
		c0 = Partc(r, deltad, sigma, noOfStepsd, s0, k);
		su = s0 + 1;
		cu = Partc(r, deltad, sigma, noOfStepsd, su, k);
		deltau = (cu - c0) / (su - s0);

		suu = su + 1;
		cuu = Partc(r, deltad, sigma, noOfStepsd, suu, k);
		deltauu = (cuu - cu) / (suu - su);

		gamma = (deltauu - deltau) / ((suu - s0)/2.0);
		answerd.push_back(gamma);
	}

	//Part e
	vector<double> answere;
	int noOfStepse = 100;
	double deltae = time / noOfStepse;
	double delta_sig = 0.001;
	double cu_1, cu_2;
	for (double s0 = 20; s0 <= 80; s0 += 2)
	{
		cu_1 = Partc(r, deltae, sigma, noOfStepse, s0, k);
		cu_2 = Partc(r, deltae, sigma + delta_sig, noOfStepse, s0, k);
		answere.push_back((cu_2 - cu_1) / delta_sig);
	}

	//Part f
	vector<double> answerf;
	int noOfStepsf = 100;
	double deltaf = time / noOfStepsf;
	double delta_r = 0.001;
	for (double s0 = 20; s0 <= 80; s0 += 2)
	{
		cu_1 = Partc(r, deltaf, sigma, noOfStepsf, s0, k);
		cu_2 = Partc(r+delta_r, deltaf, sigma, noOfStepsf, s0, k);
		answerf.push_back((cu_2 - cu_1) / delta_r);
	}

	vector<vector<double>> answers;
	answers.push_back(answera);
	answers.push_back(answerb);
	answers.push_back(answerc);
	answers.push_back(answerd);
	answers.push_back(answere);
	answers.push_back(answerf);
	writeToExcel("Q3.csv", answers);
}

