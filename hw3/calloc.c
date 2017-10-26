#include "mymalloc.h"
#include "buddy.h"
#include "utility.h"
#include <string.h>
#include <errno.h>

//void *calloc(size_t nmemb, size_t size)
void *mycalloc(size_t nmemb, size_t size)
{
    void *mem = NULL;
	size_t total_size = nmemb * size;

    if (0 >= size) {
        goto out;
    }
    //debug("Size requested %zu", nmemb * size);

    /* TODO round-off size of the next higher power of 2*/
    /* check if size can be satisfied */
    mem = mymalloc(total_size);
    //mem = malloc(total_size);
	if (NULL != mem) {
        //debug("mem %zu allocated", nmemb * size);
		memset(mem, 0, total_size);
		goto out;
    }

out:
    return mem;
}
