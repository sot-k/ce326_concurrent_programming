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
#include<error.h>
#include<unistd.h>
#include"cbuffer.h"

#define SIZE 64

volatile unsigned int wait=0; //wait is a global flag for main in order to know
//when the 2 threads are finished

typedef struct arguments{
	FILE *fp;
	long size;
} args;//data structure to be passed as thread arguments. fp is each file's
//filedescriptor. size is the total amount of bytes the reading file has.

void *readerT(void *input){
	char c;
	args *arg;
	int ret;//return value of the fread command
	arg = (args *) input;

	//printf("HELLO READ\n" );
	//we use a for, since we have the file size, in order to read the whole file
	//byte-byte
	for (size_t i = 0; i < arg->size; i++) {
		ret = fread(&c,1,1,arg->fp);
		if ( ret == 0) {//checking for errors
			perror("ERROR:");
			exit(EXIT_FAILURE);
		}
		pipe_write(c);//we call the function that writes in our pipe
	}
	pipe_close();
	wait --;//when the thread is finished, before it exectes return function
	//it reduces the wait variable by 1 in order to infor main that one thread
	//ended.
	return 0;
}
void *writerT(void *input){
	char c;
	args *arg;
	int ret;//return value of the fwrite command
	arg = (args *) input;

	//printf("HELLO WRITE\n" );
	//we use a for, since we have the file size, in order to read the whole file
	//byte-byte
	while(1) {
		ret = pipe_read(&c);//we call the function that reads from our pipe
		if(!ret){//pipe read returns 1 if it reads a char and 0 if there is no other
			//char to be read. so we break;
			break;
		}
		ret = fwrite(&c,1,1,arg->fp);
		if ( ret == 0) {//checking for errors
			perror("ERROR:");
			exit(EXIT_FAILURE);
		}
	}

	wait--;//when the thread is finished, before it exectes return function
	//it reduces the wait variable by 1 in order to infor main that one thread
	//ended.
	return 0;
}

int main(int argc, char *argv[]){
	FILE *writefp,*readfp;//our 2 filedescriptors
	pthread_t threadW,threadR;//the ids of our 2 threads
	int rvalW,rvalR;//return values to check if the threads have been created normaly
	long size;
	args readargs,writeargs;//the arguments wegive in our 2 threads

	if (argc != 3){//if we have more arguments than the expected ones we print
		//the error and we exit.
		printf("ERROR:wrong number of arguments \n");
		return 0;
	}
//initialising our pipe
	pipe_init(SIZE);
//we open the reading file
	readfp=fopen(argv[1],"r+b");
	if ( readfp == NULL) {
		perror("ERROR:");
		exit(EXIT_FAILURE);
	}
//on the reading file we use fseek with the SEEK_END option in order to go to
//the end of the file. then we use ftell which returns the current value of
//the file position indicator,which means the number of the last byte, and thus
//the size of the file. in the end we reset the filedescriptor to the start of
//our reading file.
	fseek(readfp,0,SEEK_END);
	size = ftell(readfp);
	fseek(readfp,0,SEEK_SET);
//we open the writing file
	writefp=fopen(argv[2],"w+b");
	if ( writefp == NULL ) {
		perror("ERROR:");
		exit(EXIT_FAILURE);
	}
//initialising reading arguments
	readargs.size = size;
	readargs.fp = readfp;
//initialising writing arguments
	writeargs.size = size;
	writeargs.fp = writefp;

//here we create the threads. if something goes wrong we print the error and
//we exit. if all goes well we ++ the wait global variable which is used by our
//main in order to busywait untill both threads are finished.
	rvalW=pthread_create(&threadW,NULL,writerT,(void *) &writeargs);
	if(rvalW){
		fprintf(stderr,"Error - pthread_create() return code: %d\n",rvalW);
	    exit(EXIT_FAILURE);
	}
	wait++;
	rvalR=pthread_create(&threadR,NULL,readerT,(void *) &readargs);
	if(rvalR){
		fprintf(stderr,"Error - pthread_create() return code: %d\n",rvalR);
	    exit(EXIT_FAILURE);
	}
	wait++;

	//after we create the threads we start busy waiting for them to finish.
	//we use sleep instead of just an empty loop just to not occupy the cpu
	while (wait!=0) {
		sleep(1);
	}

	//after everything is finished we wrap up closing the file descriptors
	//and lastly closing our pipe.
	fclose(writefp);
	fclose(readfp);
	pipe_destroy();
	return 0;
}
