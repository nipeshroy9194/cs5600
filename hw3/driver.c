#include "mymalloc.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    int *p = mymalloc(4);
    if (NULL == p) {
        printf("No memory allocated");
        //check if errrno is ENOMEM
    }
    *p = 12;
    printf("Sum %d",*p+1);
    int *q=mymalloc(2048);
    if (!q)
        printf("no mem for q");
    //myfree(p);
    //mycalloc(13)
    //myfree(NULL)
    return 0;
}
