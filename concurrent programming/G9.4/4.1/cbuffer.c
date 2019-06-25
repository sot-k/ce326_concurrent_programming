//#############################//
//# WRITTEN BY:               #//
//# CHOULIARAS ANDREAS   2143 #//
//# KARAMELLIOS SOTIRIOS 2237 #//
//#############################//

#include<stdio.h>
#include<malloc.h>
#include<memory.h>
#include<stdlib.h>
#include<pthread.h>
#include<sched.h>
#include "cbuffer.h"


sbuff_t * sb = NULL;
int close;
//this function initialises a pipe of size size, which is a variable that we
//pass as argument from main. it creates the circular buffer.
void pipe_init(int size){
 	sb = (sbuff_t*)malloc(sizeof(sbuff_t));
  	memset(sb, 0, sizeof(sbuff_t));
   	sb->size = size;
 	sb->buff = (char*)malloc(sizeof(char)*size);
	close = 0;

}

void pipe_close() {
	close = 1;
}
//this function writes a byte(a character) in our pipe. if the pipe has no empty
// spots it just busy waits untill an empty spot is found.
void pipe_write(char elem){

	sb->buff[sb->writingPointer] = elem;
	sb->writingPointer = (sb->writingPointer+1) % sb->size;
	sb->count ++;
}

//this function reads a byte(a character) from our pipe and then frees its position.
// if the pipe has no bytes it just busy waits untill a byte is writen.
int pipe_read(char *c){

	if (sb->count == 0) {
		return (0);
	}
	*c = sb->buff[sb->readingPointer];
	sb->readingPointer = (sb->readingPointer + 1 ) % sb->size;
	sb->count--;
	return (1);

}

//here we free the buffer.
void pipe_destroy(){
	free(sb->buff);
	free(sb);
}
