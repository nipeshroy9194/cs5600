CC=gcc
CFLAGS=

all: malloc_bfs malloc_dfs

malloc_dfs:
	$(CC) $(CFLAGS) -lm my_malloc_dfs.c -o malloc_dfs.o

malloc_bfs:
	$(CC) $(CFLAGS) -lm my_malloc_bfs.c -o malloc_bfs.o

clean:
	rm -f malloc_bfs.o malloc_dfs.o driver 2>/dev/null
