#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

/* Considering the cache size is not greater than 8MB */
int main() {
    int sum = 0;
    int i = 0, s = 0;
    double timeTaken;
    clock_t start;
    long int N = 0;
    int power_two = 12;
    int *arr = NULL;
    long int strides = 0;
    FILE *fp = NULL;
    
    fp = fopen("output.txt", "w+");
    if (fp == NULL) {
    	printf("\n Unable to open file output.txt");
    	exit(1);
	}

	/* Regime 1 */
    for (i = 0 ; i < 12; i++) {
    	power_two += i;
    	N = 1 << power_two;
		fprintf(fp, "Graph plot for %ldKB", (N/1024));
		arr = (int*) malloc (N);
		if (arr == NULL) {
			printf("\nUnable to allocate memory");
			exit(1);
		}
		strides = ((sizeof (int)) * N ) / 2;
		start = clock();
 	   	for (s = 0; s < strides; i++) {
    	    arr[i] = (s * strides) + 1;
    	    sum += arr[i];
    	}
       	timeTaken = (double)(clock() - start)/CLOCKS_PER_SEC;
    	fprintf(fp, "Time for %d: %.12f \n", sum, timeTaken);
    	free(arr);
    	arr = NULL;
	}
	
	return 0;
}
