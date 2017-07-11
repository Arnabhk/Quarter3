#pragma once
#include "Helpers.h"
#include "Generators.h"

using namespace std;

void Question1();
double Parta(double r, double delta, double sigma, int noOfSteps, double s0, double K);
double Partb(double r, double delta, double sigma, int noOfSteps, double s0, double K);
double Partc(double r, double delta, double sigma, int noOfSteps, double s0, double K, char optionType = 'c');
double Partd(double r, double delta, double sigma, int noOfSteps, double s0, double K);
double PriceOption(double r, int noOfSteps, double delta, double prob, double u, double d, double s0, double K, char optionType = 'c');
double BnlProb(unsigned n, unsigned k, double prob);
double BlackScholes_Call(double S0, double T, double X, double r, double sig);
double NCap(double x);
