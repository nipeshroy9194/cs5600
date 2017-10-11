#include "mymalloc.h"
#include "buddy.h"
#include "utility.h"
#include <errno.h>

void *mymalloc(size_t size)
{
    void *mem = NULL;

    if (0 >= size) {
        goto null_return;
    }
    debug("Size requested %zu", size);

    //check if size can be satisfied
    mem = _alloc_memory(size);
    if (NULL != mem) {
        debug("mem %zu allocated", size);
        goto mem_return;
    }

    //else request memory from kernel
    mem = _request_from_kernel(size);
    if (NULL == mem) {
        debug("requesting from kernel");
        goto null_return;
    }

mem_return:
    return mem;
null_return:
    return NULL;
}
