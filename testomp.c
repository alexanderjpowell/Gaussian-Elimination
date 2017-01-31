#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>

#define N 30
#define numThreads 2

double A[N][N], B[N], X[N];

struct reduceRowArgs {
	int arg1;
	int arg2;
};

void gauss();
void* reduceRow(void *s);

void initialize()
{
	A[0][0] = 1;
  	A[0][1] = 2;
  	A[0][2] = 3;
  	A[1][0] = 5;
  	A[1][1] = 6;
  	A[1][2] = 7;
  	A[2][0] = 9;
  	A[2][1] = 1;
  	A[2][2] = 2;
  	B[0] = 4;
  	B[1] = 8;
  	B[2] = 3;
  	X[0] = 0.0;
  	X[0] = 0.0;
  	X[0] = 0.0;
}

void randinit()
{
	double initial = drand48();
	int i, j;
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			A[i][j] = drand48();
		}
		B[i] = drand48();
		X[i] = 0.0;
	}
}



int main()
{
	int i, j;

	//initialize();
	randinit();

	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			printf("%f ", A[i][j]);
		}
		printf("\n");
	}

	for (i = 0; i < N; i++)
	{
		printf("%f\n", B[i]);
	}
	
	printf("\n");

	gauss();

	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			printf("%f ", A[i][j]);
		}
		printf("\n");
	}
	
	printf("\n");
	
	for (i = 0; i < N; i++)
	{
		printf("%f\n", X[i]);
	}

	return 0;
}





void gauss()
{
	int i, row, col;
	double ratio;
	#pragma omp parallel num_threads(2)
	for (i = 0; i < N-1; i++)
	{
		#pragma omp parallel for
		for (row = i+1; row < N; row++)
		{
			ratio = A[row][i] / A[i][i];
			for (col = i; col < N; col++)
			{
				A[row][col] -= ratio * A[i][col];
			}
			B[row] -= ratio * B[i];
		}
	}

	// Back sub
	for (row = N-1; row >= 0; row--)
	{
		X[row] = B[row];
		for (col = N-1; col > row; col--)
		{
			X[row] -= A[row][col] * X[col];
		}
		X[row] /= A[row][row];
	}
}










