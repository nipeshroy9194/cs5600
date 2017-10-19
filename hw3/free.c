#include "mymalloc.h"
#include "buddy.h"
#include "utility.h"

void myfree(void *ptr)
{
    if (NULL == ptr)
        return;
    debug("Memory for free %p", ptr);
    _reclaim_memory(ptr);
}
