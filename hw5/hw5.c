#include <stdio.h>
#include <time.h>

int main() {
    int steps = 64 * 1024 * 1024;
    int arr[1024 * 1024];
    int lengthMod = (1024 * 1024) - 1;
    int i;
    double timeTaken;
    clock_t start;

    start = clock();
    for (i = 0; i < steps; i++) {
        arr[(i * 16) & lengthMod]++;
    }
    timeTaken = (double)(clock() - start)/CLOCKS_PER_SEC;
    printf("Time for %d: %.12f \n", i, timeTaken);
}
