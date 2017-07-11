#include "Question1.h"
#include "Question2.h"
#include "Question3.h"
#include "Question4.h"
#include "Question5.h"
#include <iostream>
using namespace std;

int main()
{
	cout << "###########################################" << endl;
	cout << "Computational Finance Problem Set 3" << endl;
	cout << "Nitish Ramkumar" << endl;
	cout << "###########################################" << endl;
	
	//Question 1
	cout << endl << "QUESTION 1" << endl;
	vector<double> answers1 = Question1(1234, 5678);

	//Question 2
	cout << endl << "QUESTION 2" << endl;
	vector<double> answers2 = Question2(1234, 5678);

	//Question 3
	cout << endl << "QUESTION 3" << endl;
	double seed = 2000;
	double r = 0.04;
	double Sigma = 0.25;
	double S_0 = 15;
	double T = 0.5;
	double X = 20;
	
	Question3 q3 = Question3(seed);
	vector<double> answers3 = q3.RunQ3(S_0, T, X, r, Sigma);
	
	//Question 4
	cout << endl << "QUESTION 4" << endl;
	vector<double> answers4 = Question4(1234,5678);

	//Question 5
	cout << endl << "QUESTION 5" << endl;
	vector<double> answers5 = Question5(1234);


	int dummy;
	cout << endl << endl << "Press a key to close";
	dummy = getchar();

}

