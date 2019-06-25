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


#define LENGTH 5

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t passblue=PTHREAD_COND_INITIALIZER;
pthread_cond_t passred=PTHREAD_COND_INITIALIZER;
pthread_cond_t reply=PTHREAD_COND_INITIALIZER;

volatile int blue_queue = 0, red_queue = 0, first = 1;
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
	pthread_mutex_lock(&mutex);
	if (first != 1){
		pthread_cond_wait(&passblue,&mutex);
	}
	else{
		first = 0;
	}
	// a car passes the bridge so we -- que and cap
	blue_queue -=1;
	bridge_cap -=1;

	if ((bridge_cap > 0) && (blue_queue > 0)) {// an xwrane alloi kai an iparxoun
		// kai alloi stin oura gia na perasoun ksipna ton epomeno
		pthread_cond_signal(&passblue);
	}
	else{// flag gia na kserw oti eimai o teleutaios
		last=1;
	}
	pthread_mutex_unlock(&mutex);

	//Start crossing THE bridge
	printf("BLUE car %u Starts crossing the bridge\n", (int)pthread_self());
	sleep(LENGTH);
	printf("BLUE car %u Ended  crossing the bridge\n", (int)pthread_self());
	//End crossing THE bridge

	pthread_mutex_lock(&mutex);
	if (last == 1) {// an eimai o teleutaios
		bridge_cap=arg->cap;// arxikopoiise tin xwritikotita tis gefiras ksana
		if (red_queue > 0) {// an exei atoma i apenanti oura allakse fora kinisis
			pthread_cond_signal(&passred);
		}
		else if (blue_queue >0) {// an exei kiallous pisw apo esena ksipna ton epomeno
			pthread_cond_signal(&passblue);
		} else {// an den iparxei kaneis ase tin gefira "keni"
			first = 1;
			if (end_of_input == 1) {// an i main mas exei enimerwsei oti teleiwse
				// to input kai mas perimenei ksipna tin efoson eisai o teleutaios
				pthread_cond_signal(&reply);
			}
		}
	}
	pthread_mutex_unlock(&mutex);
	return (0);
}
void *red(void *input) {

	args *arg;
	arg = (args *) input;
	int last=0;

	printf("RED  car %u arrives at bridge\n", (int)pthread_self());
	pthread_mutex_lock(&mutex);
	if (first != 1){
		pthread_cond_wait(&passred,&mutex);
	}
	else{
		first=0;
	}
	// a car passes the bridge so we -- que and cap
	red_queue -=1;
	bridge_cap -=1;

	if ((bridge_cap > 0) && (red_queue > 0)) {// an xwrane alloi kai an iparxoun
		// kai alloi stin oura gia na perasoun ksipna ton epomeno
		pthread_cond_signal(&passred);
	}
	else{// flag gia na kserw oti eimai o teleutaios
		last=1;
	}
	pthread_mutex_unlock(&mutex);

	//Start crossing THE bridge
	printf("RED  car %u Starts crossing the bridge\n", (int)pthread_self());
	sleep(LENGTH);
	printf("RED  car %u Ended  crossing the bridge\n", (int)pthread_self());
	//End crossing THE bridge

	pthread_mutex_lock(&mutex);
	if (last == 1) {// an eimai o teleutaios
		bridge_cap=arg->cap;// arxikopoiise tin xwritikotita tis gefiras ksana
		if (blue_queue > 0) {// an exei atoma i apenanti oura allakse fora kinisis
			pthread_cond_signal(&passblue);
		}
		else if (red_queue >0) {// an exei kiallous pisw apo esena ksipna ton epomeno
			pthread_cond_signal(&passred);
		} else {// an den iparxei kaneis ase tin gefira "keni"
			first=1;
			if (end_of_input == 1) {// an i main mas exei enimerwsei oti teleiwse
				// to input kai mas perimenei ksipna tin efoson eisai o teleutaios
				pthread_cond_signal(&reply);
			}
		}
	}
	pthread_mutex_unlock(&mutex);
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
			pthread_mutex_lock(&mutex);
			blue_queue = blue_queue + 1;
			pthread_mutex_unlock(&mutex);
		}
		else if (c == 'r') {// create red car
			red_args.cap=cap;
			rval=pthread_create(&thread_red,NULL,red,(void *)&red_args);
			if(rval){
				fprintf(stderr,"Error - pthread_create() return code: %d\n",rval);
			    exit(EXIT_FAILURE);
			}
			pthread_mutex_lock(&mutex);
			red_queue = red_queue + 1;
			pthread_mutex_unlock(&mutex);
		}
		else if (c == 'q') {// end of input
			break;
		}
		sleep(1);
	}

	pthread_mutex_lock(&mutex);
	if ((red_queue != 0) || (blue_queue != 0)) {// if there are more cars in the queues
		// wait for all of them to finish
		end_of_input=1;
		pthread_cond_wait(&reply,&mutex);
	}
	pthread_mutex_unlock(&mutex);
	return 0;
}
