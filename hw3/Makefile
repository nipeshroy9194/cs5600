CC=gcc
CFLAGS=

all: malloc
	$(CC) $(CFLAGS) buddy.o malloc.o -o driver driver.c

malloc: buddy
	$(CC) $(CFLAGS) -c malloc.c

buddy:
	$(CC) $(CFLAGS) -c buddy.c

clean:
	rm -f malloc.o buddy.o driver 2>/dev/null
