CC = gcc
CFLAGS = -Wall -g
LIB = -pthread

all: trenaki.o my_sem.o
	$(CC) $(CFLAGS) trenaki.o my_sem.o -o trenaki $(LIB)

trenaki.o: trenaki.c my_sem.h
	$(CC) $(CFLAGS) -c trenaki.c

my_sem.o: my_sem.c my_sem.h
	$(CC) $(CFLAGS) -c my_sem.c

clean:
	rm -f *.o trenaki
