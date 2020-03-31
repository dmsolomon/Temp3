#include <iostream> 
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

double random(double min, double max)
{
	srand(time(NULL));
	double r= double(rand()) / RAND_MAX;
	double num= min + (max - min) * r ;
	return num;
}

