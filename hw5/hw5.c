#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

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

	fp = fopen("output", "w");
	if (fp == NULL) {
		printf("Unable to open file output.txt\n");
		exit(1);
	}

	/* Regime 1 */
	for (k = 0; k < 10; k++)
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
			fprintf(fp, "Time for %.12f\n", timeTaken);
			fflush(fp);
			free(arr);
			arr = NULL;
		}
	}

	return 0;
}
