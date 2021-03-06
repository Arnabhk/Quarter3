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