#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARRAY_SIZE    	1000
#define MB 				1024 * 1024
#define CHUNK_ALLOCATED 40 * MB

int main()
{
	void *p[ARRAY_SIZE];
	int i = 0, j = 0;
	long int ram_size = 0;
	clock_t start;
	double timeTaken;
	long int GB = 1024 * 1024 * 1024;

	for (i = 0 ; i < ARRAY_SIZE; i++)
	{
		start = clock();
		p[i] = malloc(CHUNK_ALLOCATED);
		if (p[i] == NULL)
		{
			printf("\nNo more memory available");
			goto error_out;
		}
		for (j = 0; j < i; j++)
		{
			memset(p[j], 0, CHUNK_ALLOCATED);
		}
		ram_size += CHUNK_ALLOCATED; 
		timeTaken = (double)(clock() - start)/CLOCKS_PER_SEC;
		printf("%ldMB, %.8f \n", (ram_size / (1024 * 1024)), timeTaken);
	}

error_out:
	printf("Ram Size is : %ldGB\n", (ram_size / (GB)));
	return 0;
}
