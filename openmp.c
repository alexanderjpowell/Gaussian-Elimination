#include <stdio.h>

#define N 3

int main()
{

	double A[N][N];
	double b[N];
	double ratio;
	int i, j, k;

	for (i = 0; i < N; i++)
	{
		#pragma omp parallel for
		for(j = i; j < N; j++)
		{
			ratio = A[j][i] / A[i][i];
			for(k = i; k < N; k++)
			{
				A[j][k] -= (ratio * A[i][k]);
				b[j] -= (ratio * b[i]);
			}
		}
	}
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
		printf("%f\n", b[i]);
	}

}
