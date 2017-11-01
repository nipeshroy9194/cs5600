#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARRAY_SIZE    4000
#define ONE_MB_MEMORY 1024

int main()
{
	void *p[ARRAY_SIZE];
	int i = 0, j = 0;
	long int ram_size = 0;
	clock_t start;
	double timeTaken;

	for (i = 0 ; i < ARRAY_SIZE; i++)
	{
		start = clock();
		p[i] = malloc(ONE_MB_MEMORY);
		if (p[i] == NULL)
		{
			printf("\nNo more memory available");
			goto error_out;
		}
		printf("\n");
		for (j = 0; j < i; j++)
		{
			printf("%d ",j);
			memset(p[j], 0, ONE_MB_MEMORY);
		}
		printf("\n");
		ram_size += ONE_MB_MEMORY; 
		timeTaken = (double)(clock() - start)/CLOCKS_PER_SEC;
		printf("%ldMB, %.8f \n", (ram_size / 1024), timeTaken);
	}

error_out:
	printf("Ram Size is : %ldGB\n", (ram_size / (1024 * 1024)));
	return 0;
}

int main()
{

}
