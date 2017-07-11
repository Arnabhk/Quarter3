#include "Question1.h"
#include "Question2.h"
#include "Question3.h"
#include "Question4.h"
#include "Question5.h"
#include <iostream>
using namespace std;
int main()
{
	cout << "###########################################" <<endl;
	cout << "Computational Finance Problem Set 1" << endl;
	cout << "Nitish Ramkumar"<<endl;
	cout << "###########################################" << endl;


	//Question 1
	int N = 10000;
	double seed = 2000;
	Question1(N, seed);

	//Question 2
	double probs[4] = { 0.3, 0.35, 0.2, 0.15 };
	double probVals[4] = { -1, 0, 1, 2 };
	Question2(probs, probVals, N, seed);

	//Question 3
	int n = 44;
	double p = 0.64;
	N = 1000;
	Question3(n, p, N, seed);

	//Question 4
	double lambda = 1.5;
	N = 10000;
	Question4(N, lambda, seed);

	//Question 5
	Question5(seed);

	int dummy;
	cout << endl<< endl<< "Press a key to close";
	dummy = getchar();
}


