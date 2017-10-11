#ifndef __MY_BUDDY_H
#define __MY_BUDDY_H
#include <stddef.h>
#include <stdint.h>

typedef struct BUDDY_NODE{
	void *start;
    size_t size;
	struct BUDDY_NODE *next, *prev;
	uint8_t free;
}buddy_node_t;

void *_alloc_memory(size_t size);

void *_request_from_kernel(size_t size);

#endif
