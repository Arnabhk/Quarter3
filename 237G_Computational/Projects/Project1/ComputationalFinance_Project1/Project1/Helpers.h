#pragma once
#include<vector>
#include<iostream>
#include <math.h>
#include <map>
#include <sstream>

template <typename T>
double inline mean(std::vector<T> nums)
{
	double sum = 0.0;
	for (std::vector<T>::iterator it = nums.begin(); it != nums.end(); ++it)
	{
		sum += *it;
	}
	return sum / nums.size();
}

template <typename T>
double inline stddev(std::vector<T> nums)
{
	double avg = mean(nums);
	double sum = 0.0;
	for (std::vector<T>::iterator it = nums.begin(); it != nums.end(); it++)
	{
		sum += pow(*it - avg, 2);
	}
	return pow(sum / nums.size(), 0.5);
}

using namespace std;
template <typename T>
void inline PlotHistogram(vector<T> dstbnOrig, bool isDiscrete)
{
	vector<T> dstbn(dstbnOrig.size());
	copy(dstbnOrig.begin(), dstbnOrig.end(),dstbn.begin());

	vector<T> values;
	vector<int> freq;
	sort(dstbn.begin(), dstbn.end());
	
	if (isDiscrete)
	{
		map<T, int> freq_map;
		for (int count = 0; count < dstbn.size();count++) 
		{
			++freq_map[dstbn[count]];
		}
		for (auto const & p : freq_map)
		{
			values.push_back(p.first);
			freq.push_back(p.second);
		}
	}
	else
	{
		double maxVal = (double)dstbn[dstbn.size()-1];
		double minVal = (double)dstbn[0];
		for(int i = floor(minVal);i<=ceil(maxVal);i++)
		{
			values.push_back(i);
			int freq_i = count_if(dstbn.begin(), dstbn.end(),[&](int num) { return num <= i; });
			freq.push_back(freq_i);
			dstbn.erase(remove_if(dstbn.begin(), dstbn.end(),[&](int num) {return num <= i;}), dstbn.end());
		}
	}

	//Printing
	cout << endl << "Histogram"<<endl<<endl;
	ostringstream val;
	for (int count = 0; count < values.size(); count++)
	{
		val << values[count];
		string leadingChar = isDiscrete ? val.str() : "<="+val.str();
		val.str("");
		val.clear();
		cout << leadingChar + string(5 - leadingChar.size(),' ') + "||";
		double size = (double)dstbnOrig.size() / 100.0;
		for (int freqCount = 0; freqCount < freq[count] / size; freqCount++)
		{
			cout << "*";
		}
		cout << endl;
	}
	cout << endl;


}