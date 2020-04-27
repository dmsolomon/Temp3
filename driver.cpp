#include <iostream> 
#include "mpi.h"
#include <fstream>
#include <cmath>
#include "random.h"
#include "rand_vector.h"
#include <numeric>
#include "X_matrix.h"
#include <algorithm>

using namespace std;

int main(int argc, char * argv [])
{
	MPI::Init( argc, argv );
	double tick=MPI_Wtime();
	int J=2;
	int P= MPI::COMM_WORLD.Get_size();
	int L= J*P;
	int rank= MPI::COMM_WORLD.Get_rank();
	int master=0; 
	
	//Returns centered matrix X, need to convert it 
	//to static 2D array to use MPI

	double** X_center=X_matrix(J, L, rank);
	double X_send[J][L];
	for (int i=0; i < J; i++)
	{
		for (int j=0; j < L; j++)
		{
			X_send[i][j] = X_center[i][j];
		}
	}

	for (int i=0; i < J; i++)
	{
	    delete[] X_center[i];
	}
	delete[] X_center;

	//Send each sub matrix to every processor 
	
	double X_full[L][L];
	MPI::COMM_WORLD.Barrier();
	MPI::COMM_WORLD.Allgather(X_send, J*L, MPI::DOUBLE, 
		X_full , J*L, MPI::DOUBLE);
		
	// Get each weight vector, gather on master
	double* full_weight;
	double* weight= rand_vector(rank + 1, J);
	if (rank == master)
	{
		full_weight= new double[J*P];
	}

	MPI::COMM_WORLD.Gather( weight, J, MPI::DOUBLE,
		full_weight, J, MPI::DOUBLE, master);

	// Normalize weight vector, broadcast to each processor
	double sum=0;
	double* normalized_vect=new double[L];
	if (rank==master)
	{
		double full_sum=accumulate(full_weight, full_weight + L, sum);
		for (int i=0; i < L; i++)
		{
			normalized_vect[i]=full_weight[i] / full_sum;	
		}
		double check= accumulate(normalized_vect, normalized_vect + L, sum);

	//	if ( abs(1-check) > pow(10,-10)) 
	//		MPI::COMM_WORLD.Abort(1);
		delete [] full_weight;
	}
	delete [] weight;

	MPI::COMM_WORLD.Bcast(normalized_vect, L, MPI::DOUBLE, master);

	// Compute 1 - (inner product normalized vector with itself)	
	double denom=0;
	for (int i=0; i < L; i++)
	{
		denom += normalized_vect[i] * normalized_vect[i];
	}
	denom= 1.0 - denom;
	
	// Build the J columns of matrix C for each processor 
	double** C=new double*[J];
	for (int i=0; i < J; i++)
	{
		C[i]=new double[L];
	}
	
	for (int i=0; i < J; i++)
	{
		for (int j=0; j < L; j++)
		{
			C[i][j]=0;
			for (int k=0; k < L; k++)
			{
				C[i][j] += normalized_vect[k] * X_send[i][k] *
				    X_full[j][k];
			}
			C[i][j] /= denom;
		}
	}
	
	// Print matrix C if J=2 and P=4 using all processors 
	if ( J==2 && P==4 )    
	{
		for (int i=0; i < J; i++)
		{
			for (int j=0; j < L; j++)
			{
				cout  << " " << C[i][j];
			}
			cout << endl;
		}
	}
	// Get local min, max with locations
	double* local_min=new double[3];
	double* local_max=new double[3];
	local_min[0]=*min_element(C[0], C[0] + L*J);
	local_max[0]=*max_element(C[0], C[0] + L*J);

	for (int i=0; i < J; i++)
	{
		for (int j=0; j < L; j++)
		{
			if ( C[i][j] == local_min[0])
			{
				local_min[1]=i;
				local_min[2]=j;
			}
			if ( C[i][j] == local_max[0] )
			{
				local_max[1]=i;
				local_max[2]=j;
			}
		}
	}
	for (int i=0; i < J; i++)
	{
		delete[] C[i];
	}
	delete[] C;
	
	MPI::COMM_WORLD.Barrier();

	double min;
	double max;

	// Find overall min, max and locations 
	MPI::COMM_WORLD.Allreduce(&local_min[0], &min,1, MPI::DOUBLE, MPI_MIN);
	MPI::COMM_WORLD.Allreduce(&local_max[0], &max, 1, MPI::DOUBLE, MPI_MAX);
	int min_rank;
	int max_rank;
	MPI::COMM_WORLD.Barrier();

	//Print output
	if ( local_min[0] == min ) 
	{
	    	double i_min= local_min[1] + rank*J + 1;
		double j_min= local_min[2] + 1;
	 	cout << "My rank is " << rank << " the minimum element is " << local_min[0] << 
		    " in row " << i_min << " and column " << j_min << endl;
		
	}
		
	if ( local_max[0] == max ) 
	{
	    	double i_max= local_max[1] + rank*J + 1;
		double j_max= local_max[2] + 1;
	        cout << "My rank is " << rank << " the maximum element is " << local_max[0] <<
		    " in row " << i_max << " and column " << j_max << endl;
	}
	double tock=MPI_Wtime();
	MPI::COMM_WORLD.Barrier();
	if ( rank == master )
	    cout << " The computation time is " << tock - tick << " sec" << endl;
	MPI::Finalize();
	return 0;
}

