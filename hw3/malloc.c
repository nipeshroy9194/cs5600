#include "mymalloc.h"
#include "buddy.h"
#include "utility.h"
#include <errno.h>

void *mymalloc(size_t size)
{
    void *mem = NULL;

    if (0 >= size) {
        goto out;
    }
    debug("Size requested %zu", size);

    /* TODO round-off size of the next higher power of 2*/
    //check if size can be satisfied
    mem = _alloc_memory(size);
    if (NULL != mem) {
        debug("mem %zu allocated", size);
        goto out;
    }

out:
    return mem;
}
