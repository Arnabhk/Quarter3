#pragma once
#include<vector>
#include<iostream>
#include <math.h>
#include <fstream>
using namespace std;


template <typename T>
double inline mean(vector<T> nums)
{
	double sum = 0.0;
	for (vector<T>::iterator it = nums.begin(); it != nums.end(); ++it)
	{
		sum += *it;
	}
	return sum / nums.size();
}

template <typename T>
double inline stddev(vector<T> nums)
{
	double avg = mean(nums);
	double sum = 0.0;
	for (vector<T>::iterator it = nums.begin(); it != nums.end(); it++)
	{
		sum += pow(*it - avg, 2);
	}
	return pow(sum / nums.size(), 0.5);
}

template <typename T>
void inline writeToExcel(string fileName, vector<T> vals)
{
	ofstream myfile(fileName);
	int vsize = vals.size();
	for (int n = 0; n < vsize; n++)
		myfile << vals[n] << endl;
}

template <typename T>
void inline writeToExcel(string fileName, vector<vector<T>> vals)
{
	ofstream myfile(fileName);
	int vsize = vals.size();
	for (int n = 0; n < vsize; n++)
	{
		vector<double> vec = vals[n];
		for(int j=0; j < vec.size(); j++)
		{
			myfile << vec[j] << ",";
		}
		myfile << endl;
	}
}

template <typename T>
void inline writeToExcel(string fileName, vector<T> vals1, vector<T> vals2)
{
	ofstream myfile(fileName);
	int vsize = vals1.size();
	for (int n = 0; n < vsize; n++)
		myfile << vals1[n] << ","<< vals2[n] << endl;
}