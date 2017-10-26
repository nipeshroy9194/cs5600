#include "mymalloc.h"
#include "utility.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char **argv)
{
	int *p = mymalloc(4);
    if (NULL == p) {
        debug(stdout,"No memory allocated");
        /* check if errrno is ENOMEM */
		return ENOMEM;
    }
    *p = 12;
	debug(stdout,"Sum %d\n", *p + 1);

    int *q = mymalloc(2048);
    if (NULL == q) {
        debug(stdout,"no mem for q");
		return ENOMEM;
	}
	*q = 10;
	debug(stdout,"Sum %d\n", *q + 1);

	myfree(p);
	p = NULL;

    int *f = mycalloc(1, 4);
	if (NULL == f) {
        debug(stdout,"No memory allocated");
        /* check if errrno is ENOMEM */
		return ENOMEM;
    }
	debug(stdout,"Sum %d\n", *f+1);

	int *e = mycalloc(1, 4);
	if (NULL == e) {
        debug(stdout,"No memory allocated");
        /* check if errrno is ENOMEM */
		return ENOMEM;
    }

	e[0] = 12;
	debug(stdout,"e[0]:%d\n", e[0]);

	e = myrealloc(e, 16);	
	e[0] += 1;
	e[1] = 2;
	e[2] = 3;
	e[3] = 4;
	
	debug(stdout,"e[0]:%d\n", e[0]);
	debug(stdout,"e[1]:%d\n", e[1]);
	debug(stdout,"e[2]:%d\n", e[2]);
	debug(stdout,"e[3]:%d\n", e[3]);

	myfree(q);
	q = NULL;
	myfree(f);
	f = NULL;
	myfree(e);
	e = NULL;

    return 0;
}
