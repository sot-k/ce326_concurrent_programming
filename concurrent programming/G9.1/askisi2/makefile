CC = gcc
CFLAGS = -Wall -g
LIB = -pthread

all: prime_test.o workers.o
	$(CC) $(CFLAGS) workers.o prime_test.o -o workers $(LIB)

workers.o: workers.c prime_test.h
	$(CC) $(CFLAGS) -c workers.c

prime_test.o: prime_test.c prime_test.h
	$(CC) $(CFLAGS) -c prime_test.c

clean:
	rm -f *.o workers
