#include <stdio.h> 
#include <stdlib.h>
#include <time.h>

#define KB 1024
#define MB 1024 * KB

int sizes[] = {1 * KB, 4 * KB, 8 * KB, 16 * KB,
			   32 * KB, 64 * KB, 128 * KB, 256 * KB,
    		   512 * KB, 1 * MB, 1.5 * MB, 2 * MB, 
			   2.5 * MB, 3 * MB, 3.5 * MB, 4 * MB,
			   8 * MB, 16 * MB};
int N = (sizeof(sizes)/sizeof(int));
int rerun = 5;

/* Summate the plots */
void calculate_cache_size(double plots[rerun][N], FILE *fp)
{
	int k = 0, i = 0;
	int C1 = 0, C2 = 0;
	double summation[N];
	
	fprintf(fp, "\n");
	for (i = 0; i < N; i++)
	{
		for (k = 0; k < rerun-1; k++)
		{
			summation[i] += plots[k][i];
		}
		fprintf(fp, "Plot : %ldKB %f\n", (sizes[i]/KB), summation[i]);
	}
}

int main() {
	FILE *fp = NULL;
    unsigned int strides = 256 * MB;
    static int arr[16 * MB];
    int lengthMod = 0;
    unsigned int i = 0;
    int s = 0, k = 0;
    double time_taken = 0;
    clock_t start, end;
    int N = sizeof(sizes)/sizeof(int);
    double plots[rerun][N];
    
    fp = fopen("output.txt", "w");
	if (fp == NULL) {
		printf("Unable to open file output.txt\n");
		return 1;
	}

    /* Regime 1 Saavedhra and Smith : Find Cache Size */
	/* Considering the cache size is not greater than 8MB */
	for (k = 0; k < rerun-1; k++) {
		fprintf(fp, "Plot : %d\n", k+1);
    	for (s = 0; s < N; s++) {
	    	lengthMod = sizes[s] - 1;
	    	start = clock();
	    	for (i = 0; i < strides; i++) {
	        	arr[(i * 16) & lengthMod] *= 10;
            	arr[(i * 16) & lengthMod] /= 10;
	    	}
			end = clock();
	    	time_taken = (double)(end - start)/CLOCKS_PER_SEC;
        	plots[k][s] = time_taken;
        	printf("%d, %.8f \n", (sizes[s] / KB), plots[k][s]);
			fprintf(fp, "%d, %.8f \n", (sizes[s] / KB), plots[k][s]);
			fflush(fp);
	    }
	}
	calculate_cache_size(plots, fp);

    return 0;
}
