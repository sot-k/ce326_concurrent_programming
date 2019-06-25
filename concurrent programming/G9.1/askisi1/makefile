CC = gcc
CFLAGS = -Wall -g
LIB = -pthread

all: thread_fifo.o cbuffer.o
	$(CC) $(CFLAGS) cbuffer.o thread_fifo.o -o thread_fifo $(LIB)

cbuffer.o: cbuffer.c cbuffer.h
	$(CC) $(CFLAGS) -c cbuffer.c

thread_fifo.o: thread_fifo.c cbuffer.h
	$(CC) $(CFLAGS) -c thread_fifo.c

clean:
	rm -f *.o thread_fifo
