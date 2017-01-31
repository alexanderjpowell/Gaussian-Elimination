// Alexander Powell

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>

#define n 5
#define numThr 2



double **matrix;
//double *X;
int pivotReady[n] = {0};
pthread_cond_t cond_pivot_ready;
pthread_mutex_t cond_mutex;
pthread_barrier_t our_barrier;




// returns the index in the array of the maximum value
/*
int max_loc(int input[], int len)
{
	int max = input[0];
	int location;
	for (int c = 0; c < len; c++)
	{
		if (input[c] > max)
		{
			max = input[c];
		}
	}

	int count = 0;
	for (int i = 0; i < len; i++)
	{
		if (input[i] == max)
		{
			return i;
		}
		count = count + 1;
	}

	// should never reach here
	return -1;
}
*/


//int col;
//double matrix[n][n];
//double b[n], x[n];

struct reduceRowArgs {
	int arg1;
	int arg2;
};

/*
void *reduceRow(void *arguments){
	//struct reduceRowArgs *args = arguments;
	int row = (int)arguments;
	//int row = args -> arg1;
	//int col = args -> arg2;
	double ratio = matrix[row][col] / matrix[col][col];
	for (int i = col; i < n; i++)
	{
		matrix[row][i] -= ratio * matrix[col][i];
	}
	b[row] -= ratio * b[col];
	//free(arguments);
	//return NULL;
	pthread_exit(0);
}
*/

int swapRows(int row1, int row2){
	double * tempRow = matrix[row1];
	matrix[row1] = matrix[row2];
	matrix[row2] = tempRow;
	return 0;
}

void randinit()
{
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n+1; j++)
		{
			matrix[i][j] = drand48(); //*(*(matrix + i) + j) = drand48();
			//matrix[i][j] = drand48();
		}
		//B[i] = drand48();
		//X[i] = 0.0;
	}
}




void rowReduce(void* arg);

void printMatrix(){
	int i, j;
	for(i = 0; i < n; i++){
		//printf("\nArray %d Elements: \n", i);
		for (j = 0; j < n+1; j++)
		{
			printf(" %4.2f ", matrix[i][j]);
		}
		printf("\n");
		//B[i] = drand48();
		//X[i] = 0.0;
	}
}

int main()
{

	//double matrix[n][n];
	//double b[n],x[n];
	
	//double random[5];


	//pthread_init();
	pthread_cond_init(&cond_pivot_ready, NULL);
	pthread_mutex_init(&cond_mutex, NULL);

	int i, j;

	//double B[n];
	//double X[n];
	double temp;
	//long int row;
	//void *rowptr;
	//double temp;
	//int rowsThr, rem, numRowsToReduce;
	pthread_t p_threads[numThr];
	pthread_attr_t attr;
	pthread_attr_init(&attr);



	//create matrix
	matrix = malloc(sizeof(double*)*n);
	for (i=0; i<n; i++){
		*(matrix+i) = malloc(sizeof(double)*(n+1));
	}



/*
	//test input
	matrix[0][0] = 1;
	matrix[0][1] = 2;
	matrix[0][2] = 3;
	matrix[1][0] = 5;
	matrix[1][1] = 6;
	matrix[1][2] = 7;
	matrix[2][0] = 9;
	matrix[2][1] = 1;
	matrix[2][2] = 2;

	matrix[0][3] = 4;
	matrix[1][3] = 8;
	matrix[2][3] = 3;
*/
/*
	B[0] = 4;
	B[1] = 8;
	B[2] = 3;
*/






	pthread_barrier_init(&our_barrier,NULL,numThr);

	randinit();
	//printf("Initial Matrix\n");
	//printMatrix();

	for (i=0; i<numThr; i++){
		pthread_create(&p_threads[i], &attr, rowReduce, (void *)((long)i));
	}

	for (i=0; i<numThr; i++){
		pthread_join(p_threads[i], NULL);
	}
	pthread_barrier_destroy(&our_barrier);


	// Create B
	/*
	for (i = 0; i < n; i++)
	{
		B[i] = matrix[i][n];
	}
	*/


	// Back Sub
	/*
	X[n - 1] = B[n - 1] / matrix[n - 1][n - 1];
	for (i = (n - 2); i >= 0; i--)
	{
		temp = B[i];
		for (j = (i + 1); j < n; j++)
		{
			temp -= (matrix[i][j] * X[j]);
		}
		X[i] = temp / matrix[i][i];
	}
	*/
	printf("Final Matrix:\n");
	printMatrix();

	return 0;

}

void rowReduce(void* arg){
	int id = (int)arg;
	int i, pivot, row;
	double ratio, temp, max;
	int maxRow;

	for(pivot = 0; pivot<n-1; pivot++){
		//printf("pivot %d\n", pivot);
		//pthread_mutex_lock(&cond_mutex);
		//PIVOT THREAD
		if (id == 0){
			//pthread_mutex_lock(&cond_mutex);

			//printf("PIVOT RUN\n");
			//pthread_mutex_lock(&task_cond_lock);
			//create pivot
			max = matrix[pivot][pivot]; //*(*(matrix + pivot) + pivot)
			maxRow = pivot;
			for (i = pivot+1; i < n; i++){
				temp = matrix[i][pivot]; //(*(*(matrix + i) + pivot)
				if (temp > max){
					max = temp;
					maxRow = i;
				}
			}
			//printf("max found at row %d\n", maxRow);
			swapRows(pivot, maxRow);

			//printf("Matrix after swap Rows: pivot = %d\n", pivot);
			//printMatrix();
			pivotReady[pivot] = 1;
			pthread_barrier_wait(&our_barrier);
			//pthread_cond_signal(&cond_pivot_ready);
			

			//printf("Pivot Matrix for pivot = %d\n", pivot);
			//printMatrix();

			//ELIMINATE ROWS FOR ID: 0
			//printf("iteration of piv for t1\n");
			for (row = pivot + 1 + id; row < n; row += numThr){

				ratio = matrix[row][pivot] / matrix[pivot][pivot];
				//printf("t1: row = %d, piv = %d, ratio = %f\n", row, pivot, ratio);
				for (i = pivot; i < n + 1; i++){
					matrix[row][i] -= ratio * matrix[pivot][i];
				}
				//printMatrix();
				//  sleep(1);
			}
			//pthread_mutex_unlock(&cond_mutex);
		}

		//NON-PIVOT THREAD
		else{
			pthread_barrier_wait(&our_barrier);
			//pthread_mutex_lock(&cond_mutex);
			//printf("pivotready: %d\n", pivotReady[pivot]);
			//printf("NON PIVOT RUN\n");
			//while (!(pivotReady[pivot])){
				//pthread_cond_wait(&cond_pivot_ready);
				//printf("pivotnotready hit\n");
				//(&cond_pivot_ready, &cond_mutex);
			//}
			
			for (row = pivot + 1 + id; row < n; row += numThr){
				ratio = matrix[row][pivot] / matrix[pivot][pivot];
				for (i = pivot; i < n + 1; i++){
					matrix[row][i] -= ratio * matrix[pivot][i];
				}
			}
		//pthread_mutex_unlock(&cond_mutex);

		}
	//pthread_mutex_unlock(&cond_mutex);

	}





	//printf("rowReduce called with id = %d\n", id);
	pthread_exit(0);
}

	/*
	for (int i = 0; i < 5; i++)
	{
		random[i] = drand48() * 1000;
		printf("%f\n", random[i]);
	}
	*/


	/*
	matrix[0][0] = 1;
	matrix[0][1] = 2;
	matrix[0][2] = 3;
	matrix[1][0] = 5;
	matrix[1][1] = 6;
	matrix[1][2] = 7;
	matrix[2][0] = 9;
	matrix[2][1] = 1;
	matrix[2][2] = 2;

	b[0] = 4;
	b[1] = 8;
	b[2] = 3;
	
	*/

	/*
	// Row Reduction
	for (col = 0; col < (n - 1); col++)			//col is col
	{
		printf("col: %d\n", col);
		numRowsToReduce = n - col - 1;
		rowsThr = numRowsToReduce / numThr;
		rem = numRowsToReduce % numThr;

		for (threadG = 0; threadG < rowsThr; threadG++)
		{
			for (threadN = 0; threadN < numThr; threadN++)
			{
				//a call to reduce for each thread
				row = col + 1 + threadG * numThr + threadN;
				//printf("mainB: row: %d, col: %d, rowsThr: %d\n", row, col, rowsThr);
				//struct reduceRowArgs *args = malloc(sizeof(*args));
				rowptr = (void *)row;
				//args->arg1 = row;
				//args->arg2 = col;
				pthread_create(&p_threads[threadN], &attr, reduceRow, rowptr);
				//reduceRow(row, col);
			}
		}
		for (threadN = 0; threadN < rem; threadN++)
		{
			row = col + 1 + numThr * rowsThr + threadN;
			//printf("remB: row: %d, col: %d, rowsThr: %d\n", row, col, rowsThr);
			//struct reduceRowArgs args;
			//struct reduceRowArgs *args = malloc(sizeof(*args));
			rowptr = (void *)row;
			//args->arg1 = row;
			//args->arg2 = col;
			pthread_create(&p_threads[threadN], &attr, reduceRow, rowptr);
			//reduceRow(row, col);
		}
		for (i = 0; i < numThr; i++)
		{
			pthread_join(p_threads[i], NULL);
		}

		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				printf("%f ", matrix[i][j]);
			}
			printf("\n");
		}
	*/


	
	

	// Back Substitution
	/*
	x[n - 1] = b[n - 1] / matrix[n - 1][n - 1];
	for (i = (n - 2); i >= 0; i--)
	{
		temp = b[i];
		for (j = (i + 1); j < n; j++)
		{
			temp -= (matrix[i][j] * x[j]);
		}
		x[i] = temp / matrix[i][i];
	}

	// Print Results
	printf("Answer: \n");
	for(i = 0; i < n; i++)
	{
		printf("x%d = %lf\n", i, x[i]);
	}
	*/

	// Calculate the l2-norm
	/*
	double norm;
	for (i = 0; i < n; i++)
	{
		norm = norm + (x[i] * x[i]);
	}
	norm = sqrt(norm);
	printf("L2 Norm: %f\n", norm);
	*/

	// x[n] is solution vector
	// matrix[n][n] is A vector
	// Ax is new vector A * x
	// b is rhs vector




	//////////////////////
	/*

	
	double Ax[n];
	for (i = 0; i < n; i++)
	{
		Ax[i] = 0.0;
	}
	

	// for every row
	for (i = 0; i < n; i++)
	{
		// for every column
		double prod = 0;
		for (j = 0; j < n; j++)
		{
			prod = prod + tmatrix[i][j] * x[j];
			//Ax[i][j] = prod;
		}
		Ax[i] = prod;
	}

	// printing output
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			printf("%f ", tmatrix[i][j]);
		}
		printf("\n");
	}

	for (i = 0; i < n; i++)
	{
		printf("%f\n", x[i]);
	}

	for (i = 0; i < n; i++)
	{
		printf("%f\n", Ax[i]);
	}

	for (i = 0; i < n; i++)
	{
		printf("%f\n", tb[i]);
	}

	double Ax_b[n];
	printf("Calculating Ax - b\n");
	for (i = 0; i < n; i++)
	{
		Ax_b[i] = Ax[i] - tb[i];
		printf("%f\n", Ax_b[i]);
	}

	double norm = 0;
	for (i = 0; i < n; i++)
	{
		norm = norm + (Ax_b[i] * Ax_b[i]);
	}
	norm = sqrt(norm);
	printf("l2 Norm: %f\n", norm);
	

	/////////////
	*/


	//scanf("input n for gaussian elimination. ")
	//int n = 5; // some number
	//gaussian_elim(n);
	//int test[] = {5,2,7,6,5,10,2,1,4};
	//int test[] = {2,1,3,5,4};
	//int length = (sizeof(test)/sizeof(int));
	//int largest_value = max_elem(test, length);
	//int largest_index = max_loc(test, length);
	//printf("max_elem %d\n", largest_value);

	//printf("Max: %d, Index: %d. \n", largest_value, largest_index);









