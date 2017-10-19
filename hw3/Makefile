CC=gcc
CFLAGS=

all: buddy malloc free
	$(CC) $(CFLAGS) buddy.o malloc.o free.o -o driver driver.c

malloc: malloc.c mymalloc.h buddy
	$(CC) $(CFLAGS) -c malloc.c

buddy: buddy.c buddy.h
	$(CC) $(CFLAGS) -c buddy.c

free: mymalloc.h free.c buddy
	$(CC) $(CFLAGS) -c free.c

clean:
	rm -f malloc.o buddy.o driver 2>/dev/null
