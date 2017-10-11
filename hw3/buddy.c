#include "buddy.h"
#include <unistd.h>  // for sysconf()
#include "utility.h"

//1MB initial memory
#define INITIAL_MEMORY (1*1024*1024)

buddy_node_t *root = NULL;

void *_alloc_memory(size_t size)
{
	buddy_node_t *temp = NULL;
    size_t page_size = 0;
    uint32_t pg_count = 0;
    void *mem = NULL;
	
	if (NULL == root) {
		root = sbrk(0);
        if (NULL == root) {
            debug("Failed to get brk position");
            goto null_return;
        }
        debug("Initial end of heap %p", root);
        page_size = sysconf(_SC_PAGESIZE);
        debug("page size is %zu\n", page_size);
        pg_count = INITIAL_MEMORY / page_size;  // all calculations are in bytes here
        if (pg_count < 1) 
            pg_count = 1;

        debug("Requesting %u pages\n", pg_count);
        temp = sbrk(pg_count);
        if (NULL == temp) {
            debug("Failed to increase size of heap");
            goto null_return;
        }
        debug("New end of heap %p", temp);
        mem = temp;
		//allocate size+ sizeof(buddy_node to it)
	}

	//can this node satisfy the request
	if (root->size < size) {
		//allocate size + sizeof(buddy_node)
	}

	// check if current tree has big-enough free block
	// else allocate size + sizeof(buddy_node)

mem_return:
    return mem;
null_return:
    return NULL;
}

void *_request_from_kernel(size_t size)
{
    return NULL;
}

