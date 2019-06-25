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

#define MAX_SEATS 5

#define PASS_SLEEP 0
#define REPLY 1
#define FINISHED 2
#define MUTEX 3
#define TRAIN_CLOSED 4

volatile int last = 0;
volatile int empty_seats = MAX_SEATS;
volatile int passengers = 0;
volatile int queue = 0;


typedef struct arguments{
	int semid;
	int sleep_time;
} args;


void *passenger(void *input) {

	args *arg;
	arg = (args *) input;

	mysem_down(arg->semid, PASS_SLEEP);//perimenw to trainaki
	//edw oi metablites allazoune mesa se "mutex" kathws mono enas epibatis
	//mporei na einai ksipnios kathe fora kai to traino koimatai afou perimenei
	//na ginei up to reply.
	passengers = passengers + 1;
	empty_seats = empty_seats - 1;
	printf("Passenger %u got on train\n", (int)pthread_self());
	mysem_up(arg->semid, REPLY);

	mysem_down(arg->semid, FINISHED);//perimenw na teleiwsei to trainaki gia na katebw
	printf("Passenger %u left the train\n", (int)pthread_self());
	mysem_up(arg->semid, REPLY);//enimerwnw oti katebika.

	return(0);
}

void *train(void *input){

	args *arg;
	arg = (args *) input;

	while (1) {
		while (empty_seats > 0) {
			mysem_up(arg->semid, PASS_SLEEP);//ksipnaw enan epibati gia na anebei sto trainaki
			mysem_down(arg->semid, REPLY);//perimenw epibebaiwsi oti anebike sto trainaki
			mysem_down(arg->semid, MUTEX);//mutex gia na allaksoume to queue
			queue = queue - 1;
			mysem_up(arg->semid, MUTEX);
			if ((last == 1) && (queue <= 0)) {//to last einai global flag apo tin main
				// to queue einai global metabliti gia na doume posoi akoma menoun stin oura
				break;
			}
		}
		printf("Train start\n");
		sleep(arg->sleep_time);
		printf("Train finished\n");
		for (size_t i = 0; i < passengers; i++) {
			mysem_up(arg->semid, FINISHED);//ksipnaw tous epibates na kateboun
			mysem_down(arg->semid, REPLY);//perimenw epibebaiwsi oti katebikan
		}

		sleep(arg->sleep_time);
		empty_seats = MAX_SEATS;//arxikopoiw ksana tis metablites
		passengers = 0;
		printf("Train returned to station\n");
		if ((last == 1) && (queue <= 0)) {//an den exw allous pelates bgainw
			break;
		}
	}

	mysem_up(arg->semid, TRAIN_CLOSED);//enimerwnw tin main oti teleiwsa
	return(0);

}
int main(int argc, char const *argv[]) {

	//int i = 0;
	short unsigned seminit[] = {0, 0, 0, 1, 0};
	int rval;
	char c;
	int semid, sleep_time;
	args train_arg;
	pthread_t thread_train;
	args pass_args;
	pthread_t thread_pass;

	if (argc != 2){//if we have more arguments than the expected ones. we print
		//the error and we exit.
		printf("ERROR:wrong number of arguments \n");
		return 0;
	}

	sleep_time = atoi(argv[1]);

	semid = mysem_create(5, seminit);

	train_arg.semid = semid;
	train_arg.sleep_time = sleep_time;

	rval = pthread_create(&thread_train,NULL,train,(void *)&train_arg);
	if(rval){
		fprintf(stderr,"Error - pthread_create() return code: %d\n",rval);
		exit(EXIT_FAILURE);
	}


	//pass_args = (args *)malloc(sizeof(args));
	//thread_pass = (pthread_t *)malloc(sizeof(pthread_t));

	while (1) {
		c = getchar();
		if (c == 'p') {
			//i = i + 1;
			pass_args.semid = semid;

			rval=pthread_create(&thread_pass,NULL,passenger,(void *)&pass_args);
			if(rval){
				fprintf(stderr,"Error - pthread_create() return code: %d\n",rval);
			    exit(EXIT_FAILURE);
			}

			mysem_down(semid, MUTEX);//mutex gia allagi tou global queue
			queue = queue + 1;
			mysem_up(semid, MUTEX);
			printf("Thread %u joined the queue\n", (int)thread_pass);
		}
		else if(c == 'q') {
			break;
		}
		sleep(1);
	}

	last = 1;
	if (queue == 0) {// an exei kolisei to trainaki kanoume ena up gia na ksekolisei kai
		// na katalabei oti teleiwsame giati mporei to q na erthei enw einai blockarismeno
		mysem_up(semid, REPLY);
	}
	mysem_down(semid, TRAIN_CLOSED);

	mysem_destroy(semid);

	return 0;
}
