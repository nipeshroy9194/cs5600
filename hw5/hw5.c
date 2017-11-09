#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

void calculate_cache_size(double plots[10][12])
{
	int k = 0, i = 0;
	int power_two = 12;
	int N = 0;
	int C1 = 0, C2 = 0;
	double summation[11] = {0};
	double max1 = 0;
	double max2 = 0;
	
	N = pow (2, power_two);
	for (i = 0; i < 11; i++)
	{
		for (k = 0; k < 9; k++)
		{
			summation[i] += plots[k][i];
		}
		if(max1 < summation[i])
		{
		 	max2 = max1;
        	max1 = summation[i];
		}
		else if(max2 < summation[i]) {
			max2 = summation[i];
        }
		N *= 2;
	}
	
	N = 0;
	N = pow (2, power_two);
	for (i = 0; i < 11; i++) {
		if (max1 == max2 && max1 == summation[i]) {
			C1 = C2 = (N/1024);
		}
		else {
			if (max1 == summation[i])
			{
				C1 = (N/1024);
			}
			if (max2 == summation[i])
			{
				C2 = (N/1024);
			}
		}
		N *= 2;
	}
	
	if (C1 == C2) {
		printf("\nCache Size is approximately :%d\n", C1);
	}	
	else {
		printf("\nCache Size is between : %dKB to %dKB\n", C2, C1);
	}
	printf("Check the file output.txt generated in the executable folder to see the time plots\n");
}

/* Considering the cache size is not greater than 8MB */
int main() {
	long int sum = 0;
	int i = 0, s = 0, k = 0;
	double timeTaken;
	clock_t start;
	char *arr = NULL;
	long int N = 0;
	long int strides = 0;
	int power_two = 12;
	FILE *fp = NULL;
	double plots[10][12] = {0};

	fp = fopen("output.txt", "w");
	if (fp == NULL) {
		printf("Unable to open file output.txt\n");
		exit(1);
	}

	/* Regime 1 */
	for (k = 0; k < 9; k++)
	{
		N = pow(2, power_two);
		strides = 0;
		arr = NULL;
		fprintf(fp, "\nPlot : %d\n", k+1);
		for (i = 0 ; i < 11; i++) {
			N *= 2;
			fprintf(fp, "Graph plot for %ldKB\n", (N/1024));
			fflush(fp);
			arr = (char*) malloc (N);
			if (arr == NULL) {
				printf("Unable to allocate memory\n");
				exit(1);
			}
			strides = N / 2;
			start = clock();
			for (s = 0; s < strides; s++) {
				arr[s] = (s * strides) + 1;
			}
			timeTaken = (double)(clock() - start)/CLOCKS_PER_SEC;
			fprintf(fp, "Time for %.7f\n", timeTaken);
			fflush(fp);
			plots[k][i] = timeTaken;
			free(arr);
			arr = NULL;
		}
	}
	calculate_cache_size(plots);

	return 0;
}
