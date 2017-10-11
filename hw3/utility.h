#ifndef __MY_UTILITY_H
#define __MY_UTILITY_H
#include <stdio.h>

//Debug macro
#ifndef NDEBUG
    #define debug(fmt, ...) printf(fmt"\n", ##__VA_ARGS__)
#else
    #define debug(fmt, ...)
#endif

#endif
