// Alexander Powell
// Derek O'Connell

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdint.h>
#include <time.h>

#define BILLION 1E9
#define n 3000


int numThr;
double **matrix;
//double *X;
pthread_barrier_t our_barrier;


int swapRows(int row1, int row2){
	double * tempRow = matrix[row1];
	matrix[row1] = matrix[row2];
	matrix[row2] = tempRow;
	return 0;
}

void randinit()
{
	int i, j;
	for (i = 0; i < n; i++){
		for (j = 0; j < n+1; j++){
			matrix[i][j] = drand48(); //*(*(matrix + i) + j) = drand48();
			//matrix[i][j] = drand48();
		}
	}
}

void * rowReduce(void* arg){
	int id = (intptr_t) arg;
	int i, pivot, row;
	double ratio, temp, max;
	int maxRow;

	for(pivot = 0; pivot<n-1; pivot++){
		if (id == 0){
			max = matrix[pivot][pivot];
			maxRow = pivot;
			for (i = pivot+1; i < n; i++){
				temp = matrix[i][pivot];
				if (temp > max){
					max = temp;
					maxRow = i;
				}
			}
			swapRows(pivot, maxRow);
		}

		pthread_barrier_wait(&our_barrier);
		for (row = pivot + 1 + id; row < n; row += numThr){
			ratio = matrix[row][pivot] / matrix[pivot][pivot];
			for (i = 0; i < n + 1; i++){
				matrix[row][i] -= ratio * matrix[pivot][i];
			}
		}
		pthread_barrier_wait(&our_barrier);
	}
	pthread_exit(0);
}

void printMatrix(){
	int i, j;
	for(i = 0; i < n; i++){
		//printf("\nArray %d Elements: \n", i);
		for (j = 0; j < n+1; j++)
		{
			printf(" %4.2f ", matrix[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	int i, j;
	double x[n] = {0};
	pthread_t p_threads[numThr];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	double temp;

	if (argc != 2){
		printf("run guassbar with 1 argument: the number of threads\n");
		printf("exiting.\n");
		exit(0);
	}

	numThr = atoi(argv[1]);
	if (numThr <= 0){
		printf("unacceptable number of threads passed\n");
		printf("exiting.\n");
		exit(0);
	}
	printf("Number of threads: %d\n", numThr);

	//create matrix
	matrix = malloc(sizeof(double*)*n);
	for (i=0; i<n; i++){
		*(matrix+i) = malloc(sizeof(double)*(n+1));
	}


	pthread_barrier_init(&our_barrier,NULL,numThr);

	randinit();

	struct timespec start, end;

	clock_gettime(CLOCK_REALTIME, &start);
	for (i=0; i<numThr; i++){
		pthread_create(&p_threads[i], &attr, rowReduce, (void *) (intptr_t) i);
	}

	for (i=0; i<numThr; i++){
		pthread_join(p_threads[i], NULL);
	}
	clock_gettime(CLOCK_REALTIME, &end);

	double elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

	pthread_barrier_destroy(&our_barrier);

	printf("elapsedTime: %lf\n", elapsedTime);
	
	printMatrix();
	// Back Substitution
	x[n - 1] = matrix[n - 1][n] / matrix[n - 1][n - 1];
	//printf("%f\n", x[n-1]);
	for (i = (n - 2); i >= 0; i--)
	{
		temp = matrix[i][n];
		for (j = (i + 1); j < n; j++)
		{
			temp -= (matrix[i][j] * x[j]);
		}
		x[i] = temp / matrix[i][i];
		//printf("%f, \n", x[i]);
	}
	return 0;
}

