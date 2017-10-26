#include "mymalloc.h"
#include "buddy.h"
#include "utility.h"
#include <string.h>
#include <errno.h>

//void *realloc(void *ptr, size_t size)
void *myrealloc(void *ptr, size_t size)
{
    void *mem = NULL;

    if (0 >= size) {
        goto out;
    }

    /* TODO round-off size of the next higher power of 2*/
    /* check if size can be satisfied */
    mem = mymalloc(size);
    if (NULL != mem) {
        debug("mem %zu allocated", size);
		memcpy(mem, ptr, sizeof(*ptr));
		myfree(ptr);
		goto out;
    }

out:
    return mem;
}
