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
#include "my_sem.h"

#define LENGTH 5

#define PASSBLUE 0
#define PASSRED 1
#define REPLY 2
#define MUTEX 3

volatile int blue_queue = 0, red_queue = 0;
volatile int bridge_cap = 0,end_of_input=0;
volatile char direction = 'N';

typedef struct arguments{
	int semid;
	int cap;
} args;


void *blue(void *input) {

	args *arg;
	arg = (args *) input;
	int last=0;

	printf("BLUE car %u arrives at bridge\n", (int)pthread_self());
	mysem_down(arg->semid,MUTEX);
	if (direction == 'N') {// if i am the first car in a long time change the direction
		direction='B';
	}
	mysem_up(arg->semid,MUTEX);

	do {
		mysem_down(arg->semid,PASSBLUE);
	} while(direction == 'R');

	mysem_down(arg->semid,MUTEX);// a car passes the bridge so we -- que and cap
	blue_queue -=1;
	bridge_cap -=1;
	mysem_up(arg->semid,MUTEX);

	if ((bridge_cap > 0) && (blue_queue > 0)) {// an xwrane alloi kai an iparxoun
		// kai alloi stin oura gia na perasoun ksipna ton epomeno
		mysem_up(arg->semid,PASSBLUE);
	}
	else{// flag gia na kserw oti eimai o teleutaios
		last=1;
	}

	//Start crossing THE bridge
	printf("BLUE car %u Starts crossing the bridge\n", (int)pthread_self());
	sleep(LENGTH);
	printf("BLUE car %u Ended  crossing the bridge\n", (int)pthread_self());
	//End crossing THE bridge

	mysem_down(arg->semid,MUTEX);
	if (last == 1) {// an eimai o teleutaios
		bridge_cap=arg->cap;// arxikopoiise tin xwritikotita tis gefiras ksana
		if (red_queue > 0) {// an exei atoma i apenanti oura allakse fora kinisis
			direction='R';
			mysem_up(arg->semid,PASSRED);
		}
		else if (blue_queue >0) {// an exei kiallous pisw apo esena ksipna ton epomeno
			mysem_up(arg->semid,PASSBLUE);
		} else {// an den iparxei kaneis ase tin gefira "keni"
			direction='N';
			if (end_of_input == 1) {// an i main mas exei enimerwsei oti teleiwse
				// to input kai mas perimenei ksipna tin efoson eisai o teleutaios
				mysem_up(arg->semid,REPLY);
			}
		}
	}
	mysem_up(arg->semid,MUTEX);
	return (0);
}
void *red(void *input) {

	args *arg;
	arg = (args *) input;
	int last=0;

	printf("RED  car %u arrives at bridge\n", (int)pthread_self());
	mysem_down(arg->semid,MUTEX);
	if (direction == 'N') {// if i am the first car in a long time change the direction
		direction='R';
	}
	mysem_up(arg->semid,MUTEX);

	do {
		mysem_down(arg->semid,PASSRED);
	} while(direction == 'B');

	mysem_down(arg->semid,MUTEX);// a car passes the bridge so we -- que and cap
	red_queue -=1;
	bridge_cap -=1;
	mysem_up(arg->semid,MUTEX);

	if ((bridge_cap > 0) && (red_queue > 0)) {// an xwrane alloi kai an iparxoun
		// kai alloi stin oura gia na perasoun ksipna ton epomeno
		mysem_up(arg->semid,PASSRED);
	}
	else{// flag gia na kserw oti eimai o teleutaios
		last=1;
	}

	//Start crossing THE bridge
	printf("RED  car %u Starts crossing the bridge\n", (int)pthread_self());
	sleep(LENGTH);
	printf("RED  car %u Ended  crossing the bridge\n", (int)pthread_self());
	//End crossing THE bridge

	mysem_down(arg->semid,MUTEX);
	if (last == 1) {// an eimai o teleutaios
		bridge_cap=arg->cap;// arxikopoiise tin xwritikotita tis gefiras ksana
		if (blue_queue > 0) {// an exei atoma i apenanti oura allakse fora kinisis
			direction='B';
			mysem_up(arg->semid,PASSBLUE);
		}
		else if (red_queue >0) {// an exei kiallous pisw apo esena ksipna ton epomeno
			mysem_up(arg->semid,PASSRED);
		} else {// an den iparxei kaneis ase tin gefira "keni"
			direction='N';
			if (end_of_input == 1) {// an i main mas exei enimerwsei oti teleiwse
				// to input kai mas perimenei ksipna tin efoson eisai o teleutaios
				mysem_up(arg->semid,REPLY);
			}
		}
	}
	mysem_up(arg->semid,MUTEX);

	return(0);
}

int main(int argc, char const *argv[]) {

	char c;
	int rval,cap,semid;
	short unsigned seminit[] = {1, 1, 0, 1};
	pthread_t thread_blue, thread_red;
	args blue_args, red_args;

	if (argc != 2){//if we have more arguments than the expected ones. we print
		//the error and we exit.
		printf("ERROR:wrong number of arguments \n");
		return 0;
	}

	cap=atoi(argv[1]);
	bridge_cap=cap;

	semid=mysem_create(4,seminit);

	while (1) {
		c = getchar();

		if (c == 'b') {// create blue car
			blue_args.semid=semid;
			blue_args.cap=cap;
			rval=pthread_create(&thread_blue,NULL,blue,(void *)&blue_args);
			if(rval){
				fprintf(stderr,"Error - pthread_create() return code: %d\n",rval);
			    exit(EXIT_FAILURE);
			}
			mysem_down(semid, MUTEX);
			blue_queue = blue_queue + 1;
			mysem_up(semid, MUTEX);
		}
		else if (c == 'r') {// create red car
			red_args.semid=semid;
			red_args.cap=cap;
			rval=pthread_create(&thread_red,NULL,red,(void *)&red_args);
			if(rval){
				fprintf(stderr,"Error - pthread_create() return code: %d\n",rval);
			    exit(EXIT_FAILURE);
			}
			mysem_down(semid, MUTEX);
			red_queue = red_queue + 1;
			mysem_up(semid, MUTEX);
		}
		else if (c == 'q') {// end of input
			break;
		}
		sleep(1);
	}

	mysem_down(semid, MUTEX);
	if ((red_queue != 0) || (blue_queue != 0)) {// if there are more cars in the queues
		// wait for all of them to finish
		end_of_input=1;
		mysem_up(semid, MUTEX);
		mysem_down(semid, REPLY);
		mysem_down(semid, MUTEX);
	}
	mysem_up(semid, MUTEX);

	mysem_destroy(semid);
	return 0;
}
