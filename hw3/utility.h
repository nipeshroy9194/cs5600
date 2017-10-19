#ifndef __MY_UTILITY_H
#define __MY_UTILITY_H
#include <stdio.h>

//Debug macro
#ifndef NDEBUG
    #define debug(fmt, ...) printf(fmt" f(): %s\n", ##__VA_ARGS__, __func__)
#else
    #define debug(fmt, ...)
#endif

#endif
