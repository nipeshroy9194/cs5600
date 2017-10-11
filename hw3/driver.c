#include "mymalloc.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    int *p = mymalloc(sizeof(int));
    if (NULL == p) {
        printf("No memory allocated");
        //check if errrno is ENOMEM
    }
    //mycalloc(13)
    //myfree(NULL)
    return 0;
}
