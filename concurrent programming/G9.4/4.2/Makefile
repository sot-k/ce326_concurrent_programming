CC = gcc
CFLAGS = -Wall -g
LIB =

all: mythreads.o coroutines.o lists.o workers.o prime_test.o
	$(CC) $(CFLAGS) mythreads.o coroutines.o lists.o prime_test.o workers.o -o workers $(LIB)
	rm mythreads.o coroutines.o lists.o workers.o prime_test.o

workers.o: workers.c lib/mythreads.h lib/coroutines.h lib/lists.h prime_test.h
	$(CC) $(CFLAGS) -c workers.c

mythreads.o: lib/mythreads.c lib/mythreads.h
	$(CC) $(CFLAGS) -c lib/mythreads.c

coroutines.o: lib/coroutines.c lib/coroutines.h
		$(CC) $(CFLAGS) -c lib/coroutines.c

lists.o: lib/lists.c lib/lists.h
		$(CC) $(CFLAGS) -c lib/lists.c

prime_test.o: prime_test.c prime_test.h
		$(CC) $(CFLAGS) -c prime_test.c

clean:
	rm -f *.o workers
