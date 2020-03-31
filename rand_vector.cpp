#include <iostream> 
#include "random.h" 
using namespace std;

double* rand_vector(double K, int J)
{
	double* vect=new double[J];
	for (int i=0; i < J; i++)
	{
 		vect[i]=random(K/double(i+1), double(i+1)*K + 1.0);		
	}
	return vect;
}

