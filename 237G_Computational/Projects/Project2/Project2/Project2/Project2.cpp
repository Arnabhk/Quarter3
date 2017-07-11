#include "Question1.h"
#include "Question2.h"
#include "Question3.h"
#include "Question4.h"
#include "Question5.h"
#include "Question6.h"
#include <iostream>
using namespace std;

int main()
{
	cout << "###########################################" << endl;
	cout << "Computational Finance Problem Set 2" << endl;
	cout << "Nitish Ramkumar" << endl;
	cout << "###########################################" << endl;


	//Question 1
	cout << endl << "QUESTION 1" << endl;
	double seed = 2000;
	double answer1 = Question1(seed);
	
	//Question 2
	cout << endl << "QUESTION 2" << endl;
	double answer2 = Question2(seed);

	//Question 3
	cout << endl << "QUESTION 3" << endl;
	vector<double> answers3 = Question3(seed);

	//Question 4
	cout << endl << "QUESTION 4" << endl;
	vector<double> answers4 = Question4(seed);
	
	//Question 5
	cout << endl << "QUESTION 5" << endl;
	Question5(seed);

	//Question 6
	cout << endl << "QUESTION 6" << endl;
	vector<double> answers6 = Question6(seed);

	int dummy;
	cout << endl << endl << "Press a key to close";
	dummy = getchar();
}

