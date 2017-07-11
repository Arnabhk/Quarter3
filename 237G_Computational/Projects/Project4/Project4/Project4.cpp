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
	cout << "Computational Finance Problem Set 4" << endl;
	cout << "Nitish Ramkumar" << endl;
	cout << "###########################################" << endl;
	
	//Question 1
	cout << endl << "QUESTION 1" << endl;
	Question1();

	//Question 2
	cout << endl << "QUESTION 2" << endl;
	double s0 = 905.96;
	double K = 910;
	Question2();

	//Question 3
	cout << endl << "QUESTION 3" << endl;
	Question3();
	
	//Question 4
	cout << endl << "QUESTION 4" << endl;
	Question4();

	//Question 5
	cout << endl << "QUESTION 5" << endl;
	Question5();

	//Question 6
	cout << endl << "QUESTION 6" << endl;
	Question6();


	int dummy;
	cout << endl << endl << "Press a key to close";
	dummy = getchar();

}

