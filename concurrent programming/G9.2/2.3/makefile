CC = gcc
CFLAGS = -Wall -g
LIB = -pthread

all: gefira.o my_sem.o
	$(CC) $(CFLAGS) gefira.o my_sem.o -o gefira $(LIB)

gefira.o: gefira.c my_sem.h
	$(CC) $(CFLAGS) -c gefira.c

my_sem.o: my_sem.c my_sem.h
	$(CC) $(CFLAGS) -c my_sem.c

clean:
	rm -f *.o gefira
