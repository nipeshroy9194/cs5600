#ifndef __MY_MALLOC_H
#define __MY_MALLOC_H
#include <stddef.h>  //for size_t

void *mymalloc(size_t size);
void myfree(void *ptr);
void *mycalloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
struct mallinfo mallinfo();
void malloc_stats();

#endif
