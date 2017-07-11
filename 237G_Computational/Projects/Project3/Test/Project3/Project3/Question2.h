#pragma once
#include "Helpers.h"
#include "Generators.h"
using namespace std;

vector<double> Question2(double seed1, double seed2);
vector<double> GenerateXPath(double X0, double stepSize, double Time, double seed1, double seed2);
vector<double> GenerateYPath(double Y0, double stepSize, double Time, double seed1, double seed2);