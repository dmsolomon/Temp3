#include <iostream>
#include "random.h"

using namespace std;
//Returns J by L block of X that has been centered 
double** X_matrix(int J, int L, int proc)
{
	double** X= new double*[J];
	double** center_X=new double*[J];
	for (int i=0; i < J; i++)
	{
		X[i]=new double[L];
		center_X[i]=new double[L];
		for (int j=0; j < L; j++)
		{
		    	double ii=double(i);
			double jj=double(j);
			X[i][j]=random((-ii+1+proc*double(J))*(jj+1), double(ii+1+proc*J)/double(j+1));
		}
	}

	double* means=new double[J];
	for (int i=0; i < J; i++)
	{
		for (int j=0; j < L; j++)
		{
			means[i] += X[i][j];
		}
		means[i]=means[i]/double(L);
	}
	
	for (int i=0; i < J; i++)
	{
		for (int j=0; j < L; j++)
		{
			center_X[i][j] = X[i][j] - means[i];
		}
	}
	return center_X;
}


