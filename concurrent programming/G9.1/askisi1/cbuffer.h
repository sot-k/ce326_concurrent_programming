//#############################//
//# WRITTEN BY:               #//
//# CHOULIARAS ANDREAS   2143 #//
//# KARAMELLIOS SOTIRIOS 2237 #//
//#############################//

#ifndef CBUFFER
#define CBUFFER

#include<stdio.h>
#include<malloc.h>
#include<memory.h>
#include<stdlib.h>


typedef struct sbuff_{
	char * buff;
 	int readingPointer;
 	int writingPointer;
 	int size;
 	int count;
} sbuff_t;

void pipe_init(int size);

void pipe_write(char elem);

int pipe_read(char *c);

void pipe_close();

void pipe_destroy();

#endif
