CC=gcc
DEBUG=no
CFLAGS=-fPIC -Wall
CFLAGS_ON_DEBUG=-DDEBUG -fPIC -Wall -Werror

all: lib driver test

ifeq ($(DEBUG),no)
test:
	$(CC) t-test1.c -lpthread -o test

driver:
	$(CC) $(CFLAGS) -L./ driver.c -lmalloc -o driver

lib: malloc buddy free calloc realloc
	$(CC) -shared -o libmalloc.so buddy.o malloc.o calloc.o realloc.o free.o

malloc: mymalloc.h buddy utility.h
	$(CC) $(CFLAGS) -c malloc.c

calloc: mymalloc.h buddy utility.h
	$(CC) $(CFLAGS) -c calloc.c

realloc: mymalloc.h buddy utility.h
	$(CC) $(CFLAGS) -c realloc.c

free: mymalloc.h buddy utility.h
	$(CC) $(CFLAGS) -c free.c

buddy: buddy.h utility.h
	$(CC) $(CFLAGS) -c buddy.c
else
test:
	$(CC) t-test1.c -lpthread -o test

driver:
	$(CC) $(CFLAGS_ON_DEBUG) -L./ driver.c -lmalloc -o driver

lib: malloc buddy free calloc realloc
	$(CC) -shared -o libmalloc.so buddy.o malloc.o calloc.o realloc.o free.o

malloc: mymalloc.h buddy utility.h
	$(CC) $(CFLAGS_ON_DEBUG) -c malloc.c

calloc: mymalloc.h buddy utility.h
	$(CC) $(CFLAGS_ON_DEBUG) -c calloc.c

realloc: mymalloc.h buddy utility.h
	$(CC) $(CFLAGS_ON_DEBUG) -c realloc.c

free: mymalloc.h buddy utility.h
	$(CC) $(CFLAGS_ON_DEBUG) -c free.c

buddy: buddy.c buddy.h utility.h
	$(CC) $(CFLAGS_ON_DEBUG) -c buddy.c
endif

clean:
	rm -f test libmalloc.so buddy.o malloc.o realloc.o free.o calloc.o driver 2>/dev/null
