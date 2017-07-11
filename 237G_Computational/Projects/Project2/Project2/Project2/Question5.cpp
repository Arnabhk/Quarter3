#include "Question5.h"
#include <Windows.h>
void Question5(double seed)
{
	double S0 = 88;
	double r = 0.04;
	double sigma = 0.18;
	int NoOfSim = 1000;

	//a
	//Calculate mean of paths
	cout << endl << "a)" << endl;
	vector<double> means = CalculateMeanOfPaths(S0, r, sigma, NoOfSim, seed);
	
	const char* path = "Excels";
	CreateDirectoryA(path,NULL);

	writeToExcel("Excels/means1.csv", means);
	cout << endl << "The expected value for sigma = 0.18 for various time periods is " << endl;
	for (const auto& i : means)
		std::cout << i << ' ';
	cout << "Graphs on pdf" << endl;

	//b
	vector<vector<double>> allPaths = CalculateAllStockPaths(S0, r, sigma, 10, seed, 1.0/1000.0, 6);
	for(int count = 0 ; count <allPaths.size(); count++)
	{
		ostringstream s;
		s << count + 1;
		vector<double> paths = allPaths[count];
		writeToExcel("Excels/stocks" + s.str() + ".csv", paths);
	}
	//c
	cout << endl << "c) " << endl;
	cout << "Graphs on pdf" << endl;

	//d
	double sigma2 = 0.35;
	vector<double> means2 = CalculateMeanOfPaths(S0, r, sigma2, NoOfSim, seed);
	writeToExcel("Excels/means2.csv", means2);
	
	vector<vector<double>> allPaths2 = CalculateAllStockPaths(S0, r, sigma2, 10, seed, 1.0 / 1000.0, 6);
	for (int count = 0; count <allPaths2.size(); count++)
	{
		ostringstream s;
		s << count + 1;
		vector<double> paths = allPaths2[count];
		writeToExcel("Excels/stocks" + s.str() + "_2.csv", paths);
	}

	cout << endl<< "d) " << endl;
	cout << endl << "The expected value for sigma = 0.35 for various time periods is "<<endl;
	for (const auto& i : means2)
		std::cout << i << ' ';

	cout << endl << " For this sample, the prices have increased with increase in sigma." << endl;
	int NoOfSim_2 = 60000;
	vector<double> means_att2 = CalculateMeanOfPaths(S0, r, sigma, NoOfSim_2, seed);
	vector<double> means2_att2 = CalculateMeanOfPaths(S0, r, sigma2, NoOfSim_2, seed);
	cout << endl << "But when I increase the Number of simulations, the expected value converges to the same value." << endl;
	cout << "This is as per the theoretical expected value exp(rT), in which there is no volatility term" <<endl;
	cout << "Here are the expected values for 60000 iterations" << endl;
	cout << endl << "0.18 Sigma (60000 sim): ";
	for (const auto& i : means_att2)
		std::cout << i << ' ';
	cout << endl << "0.35 Sigma (60000 sim): ";
	for (const auto& i : means2_att2)
		std::cout << i << ' ';
	writeToExcel("Excels/means1_Big.csv", means_att2);
	writeToExcel("Excels/means2_Big.csv", means2_att2);

	cout << endl << endl << "For the stock paths, the increase in sigma, increases the randomness of the paths and makes the swings more wild" << endl;
	cout << "Check pdf for graphs"<<endl;

}

vector<double> CalculateMeanOfPaths(double S0, double r, double sigma, int NoOfSim, double seed)
{
	vector<double> randNums = GenerateNormalDistn(NoOfSim, seed);
	vector<double> means;
	vector<double> st;
	for (int count = 1; count <= 10; count++)
	{
		st = GenerateStockPath(count, S0, r, sigma, NoOfSim, randNums);
		means.push_back(mean(st));
	}
	return(means);
}

vector<vector<double>> CalculateAllStockPaths(double S0, double r, double sigma, double Time, double seed, double stepSize, int noOfPaths)
{
	vector<vector<double>> allStockPaths;
	for (int pathCount = 0; pathCount < noOfPaths; pathCount++)
	{
		vector<double> stockPath = GenerateFullStockPath(Time, S0, r, sigma, Time*stepSize, seed*(pathCount+1));
		allStockPaths.push_back(stockPath);
	}

	return allStockPaths;
}

