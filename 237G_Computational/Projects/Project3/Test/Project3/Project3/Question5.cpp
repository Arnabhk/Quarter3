#define _USE_MATH_DEFINES
#include <cmath>
#include "Question5.h"
using namespace std;

vector<double> Question5(double seed)
{
	int N = 100;

	//a
	vector<double> uni_rand = GenerateUniformDist(N * 2, seed);
	vector<vector<double>> uni_vectors;
	int count = 0;
	for (int i = 0; i < N; i++) 
	{
		vector<double> vec;
		vec.push_back(uni_rand[count]);
		count = count + 1;
		vec.push_back(uni_rand[count]);
		count = count + 1;
		uni_vectors.push_back(vec);
	}
	writeToExcel("Excels/uniformDist.csv", uni_vectors);

	//b
	vector<double> seq1 = HaltonSequence(100, 2);
	vector<double> seq2 = HaltonSequence(100, 7);
	writeToExcel("Excels/HaltonSeq1.csv", seq1, seq2);
	//c
	vector<double> seq3 = HaltonSequence(100, 4);
	writeToExcel("Excels/HaltonSeq2.csv", seq1, seq3);

	//e
	int Tot = 10000;
	double answer1 = CalculateIntegral(2, 4, Tot);
	double answer2 = CalculateIntegral(2, 7, Tot);
	double answer3 = CalculateIntegral(5, 7, Tot);
	cout << endl << "Answer for Integral with bases 2 and 4 = " << answer1 << endl;
	cout << "Answer for Integral with bases 2 and 7 = " << answer2 << endl;
	cout << "Answer for Integral with bases 5 and 7 = " << answer3 << endl;

	vector<double> answers{ answer1, answer2, answer3 };
	return answers;

}

vector<double> HaltonSequence(int k, int m)
{
	vector<double> sequence(k, 0);
	double r = ceil(log(k) / log(m)) + 1;
	vector<double> baseRep(r, 0);
	for (int count = 0; count < k; count++)
	{
		int j = 0;
		bool done = false;
		while (!done)
		{
			baseRep[j] = baseRep[j] + 1;
			if (baseRep[j] < m){done = true;}
			else
			{
				baseRep[j] = 0;
				j++;
			}
		}
		for (int baseCt = 0; baseCt < r; baseCt++)
		{
			sequence[count] = sequence[count] + (baseRep[baseCt] / pow(m, baseCt+1));
		}
	}
	return sequence;
}

double CalculateIntegral(int m1, int m2, int k)
{
	vector<double> seq1 = HaltonSequence(k, m1);
	vector<double> seq2 = HaltonSequence(k, m2);

	vector<double> intVal;
	for (int count = 0; count < k; count++)
	{
		double val = exp(-1.0 * seq1[count] * seq2[count])* (sin(6 * M_PI * seq1[count]) + cbrt(cos(2 * M_PI * seq2[count])));
		intVal.push_back(val);
	}
	return mean(intVal);
}