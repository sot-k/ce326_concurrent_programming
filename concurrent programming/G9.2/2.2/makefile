CC = gcc
CFLAGS = -Wall -g
LIB = -pthread

all: prime_test.o workers.o my_sem.o
	$(CC) $(CFLAGS) workers.o prime_test.o my_sem.o -o workers $(LIB)

workers.o: workers.c prime_test.h
	$(CC) $(CFLAGS) -c workers.c

prime_test.o: prime_test.c prime_test.h
	$(CC) $(CFLAGS) -c prime_test.c

my_sem.o: my_sem.c my_sem.h
	$(CC) $(CFLAGS) -c my_sem.c

clean:
	rm -f *.o workers
