#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool validate_and_get_args(int argc,
		char **argv,
		int *x,
		int *n)
{
	int ch;

	if (argc == 1) {
		printf("Supply arguments with -x and -n\n");
		exit(1);
	}

	while((ch = getopt(argc, argv, "x:n:")) != -1)
	{
		switch(ch) {
			case 'x':
				*x = atoi(optarg);
				break;
			case 'n':
				*n = atoi(optarg);
				break;
			default:
				if (n < 0)
					printf("Invalid arguments\n");
				return false;
		}
	}

	return true;
}

int factorial(int n)
{
	if (n == 0)
		return 1;
	if (n == 1)
		return 1;
	return n * factorial(n - 1);
}

int power(int x, int n)
{
	int answer = x;

	if (n == 1)
		goto out;

	while(n > 1) {
		answer = answer * x;
		n--;
	}

out:
	return answer;
}

int main(int argc, char **argv)
{
	int x = -1, n = -1, numerator = 0, denominator = 0;
	float answer = 0;
	bool ret = false;

	ret = validate_and_get_args(argc, argv, &x, &n);
	if (ret == false) {
		printf("Usage ./worker.o -x number -n number\n");
		return 1;
	}

	numerator = power(x, n);
	denominator = factorial(n);

	answer = (float)numerator/(float)denominator;
	write(1, &answer, sizeof(answer));
}
