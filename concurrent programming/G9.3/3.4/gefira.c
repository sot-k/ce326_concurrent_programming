//#############################//
//# WRITTEN BY:               #//
//# CHOULIARAS ANDREAS   2143 #//
//# KARAMELLIOS SOTIRIOS 2237 #//
//#############################//
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <pthread.h>
#include <error.h>
#include <unistd.h>

#include "ccr.h"

CCR_DECLARE(CR);

#define LENGTH 5

volatile int blue_queue = 0, red_queue = 0;
volatile int bridge_cap = 0,end_of_input=0;
volatile char direction = 'N';

typedef struct arguments{
	int cap;
} args;


void *blue(void *input) {

	args *arg;
	arg = (args *) input;
	int last=0;

	printf("BLUE car %u arrives at bridge\n", (int)pthread_self());
	CCR_EXEC(CR,1,{
		if (direction=='N') {
			direction='B';
		}
	});
	CCR_EXEC(CR,(direction=='B' && bridge_cap > 0),{
		blue_queue -=1;
		bridge_cap -=1;
		if (bridge_cap==0 || blue_queue==0) {
			last=1;
			bridge_cap = 0;
		}
	});

	//Start crossing THE bridge
	printf("BLUE car %u Starts crossing the bridge\n", (int)pthread_self());
	fflush(stdout);
	sleep(LENGTH-1);
	printf("BLUE car %u Ended  crossing the bridge\n", (int)pthread_self());
	fflush(stdout);
	sleep(1);
	//End crossing THE bridge

	CCR_EXEC(CR,1,{
		if (last == 1) {// an eimai o teleutaios
			bridge_cap=arg->cap;// arxikopoiise tin xwritikotita tis gefiras ksana
			if (red_queue > 0) {// an exei atoma i apenanti oura allakse fora kinisis
				direction='R';
			}
			else if (blue_queue >0) {// an exei kiallous pisw apo esena ksipna ton epomeno
				direction='B';
			} else {// an den iparxei kaneis ase tin gefira "keni"
				direction='N';
			}
		}
	});
	return (0);
}
void *red(void *input) {

	args *arg;
	arg = (args *) input;
	int last=0;

	printf("RED  car %u arrives at bridge\n", (int)pthread_self());
	CCR_EXEC(CR,1,{
		if (direction=='N') {
			direction='R';
		}
	});
	CCR_EXEC(CR,(direction=='R' && bridge_cap > 0 ),{
		red_queue -=1;
		bridge_cap -=1;
		if (bridge_cap==0 || red_queue==0) {
			last=1;
			bridge_cap = 0;
		}
	});

	//Start crossing THE bridge
	printf("RED  car %u Starts crossing the bridge\n", (int)pthread_self());
	fflush(stdout);
	sleep(LENGTH-1);
	printf("RED  car %u Ended  crossing the bridge\n", (int)pthread_self());
	fflush(stdout);
	sleep(1);
	//End crossing THE bridge

	CCR_EXEC(CR,1,{
		if (last == 1) {// an eimai o teleutaios
			bridge_cap=arg->cap;// arxikopoiise tin xwritikotita tis gefiras ksana
			if (blue_queue > 0) {// an exei atoma i apenanti oura allakse fora kinisis
				direction='B';
			}
			else if (red_queue >0) {// an exei kiallous pisw apo esena ksipna ton epomeno
				direction='R';
			} else {// an den iparxei kaneis ase tin gefira "keni"
				direction='N';
			}
		}
	});
	return (0);
}

int main(int argc, char const *argv[]) {

	char c;
	int rval,cap;
	pthread_t thread_blue, thread_red;
	args blue_args, red_args;

	if (argc != 2){//if we have more arguments than the expected ones. we print
		//the error and we exit.
		printf("ERROR:wrong number of arguments \n");
		return 0;
	}
	CCR_INIT(CR);

	printf("Enter car sequence r-red,b-blue, Always end with q to quit \n");
	cap=atoi(argv[1]);
	bridge_cap=cap;

	while (1) {
		c = getchar();

		if (c == 'b') {// create blue car
			blue_args.cap=cap;
			rval=pthread_create(&thread_blue,NULL,blue,(void *)&blue_args);
			if(rval){
				fprintf(stderr,"Error - pthread_create() return code: %d\n",rval);
			    exit(EXIT_FAILURE);
			}
			CCR_EXEC(CR,1,{blue_queue = blue_queue + 1;})
		}
		else if (c == 'r') {// create red car
			red_args.cap=cap;
			rval=pthread_create(&thread_red,NULL,red,(void *)&red_args);
			if(rval){
				fprintf(stderr,"Error - pthread_create() return code: %d\n",rval);
			    exit(EXIT_FAILURE);
			}
			CCR_EXEC(CR,1,{red_queue = red_queue + 1;})
		}
		else if (c == 'q') {// end of input
			break;
		}
		sleep(1);
	}

	CCR_EXEC(CR,(direction=='N' && red_queue == 0 && blue_queue == 0), printf("Terminating...\n"););
	return 0;
}
