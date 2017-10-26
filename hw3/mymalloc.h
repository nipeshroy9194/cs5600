#ifndef __MY_MALLOC_H
#define __MY_MALLOC_H
#include <stddef.h>  //for size_t

/**
 * Implement the below functionality and add hooks for the same
 * but make sure printf and others are not executed as they use malloc()
 * internally
 */
#if 0
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
struct mallinfo mallinfo();
void malloc_stats();
#else
void *mymalloc(size_t size);
void myfree(void *ptr);
void *mycalloc(size_t nmemb, size_t size);
void *myrealloc(void *ptr, size_t size);
struct mallinfo mallinfo();
void malloc_stats();
#endif

#endif
