CC = gcc
CFLAGS = -Wall -g
LIB = 

all: context_fifo.o coroutines.o cbuffer.o
	$(CC) $(CFLAGS) context_fifo.o coroutines.o cbuffer.o -o context_fifo $(LIB)
	rm context_fifo.o coroutines.o cbuffer.o

context_fifo.o: context_fifo.c coroutines.h
	$(CC) $(CFLAGS) -c context_fifo.c

coroutines.o: coroutines.c coroutines.h
		$(CC) $(CFLAGS) -c coroutines.c

cbuffer.o: cbuffer.c cbuffer.h
		$(CC) $(CFLAGS) -c cbuffer.c

clean:
	rm -f *.o context_fifo
