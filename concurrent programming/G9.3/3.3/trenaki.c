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

#define MAX_SEATS 5

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pass_sleep=PTHREAD_COND_INITIALIZER;
pthread_cond_t finished=PTHREAD_COND_INITIALIZER;
pthread_cond_t start=PTHREAD_COND_INITIALIZER;

volatile int last = 0,first=1;
volatile int empty_seats = MAX_SEATS;
volatile int passengers = 0;
volatile int queue = 0;


typedef struct arguments{
	int sleep_time;
} args;


void *passenger(void *input) {

	//edw oi metablites allazoune mesa se "mutex" kathws mono enas epibatis
	//mporei na einai ksipnios kathe fora kai to traino koimatai afou perimenei
	//na ginei up to reply.
	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&start);
	pthread_mutex_unlock(&mutex);

	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&pass_sleep,&mutex);
	pthread_mutex_unlock(&mutex);

	printf("Passenger %u got on train\n", (int)pthread_self());


	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&finished,&mutex); //perimenw na teleiwsei to trainaki gia na katebw
	printf("Passenger %u left the train\n", (int)pthread_self());
	pthread_mutex_unlock(&mutex);//enimerwnw oti katebika.

	return(0);
}

void *train(void *input){

	args *arg;
	arg = (args *) input;
	int i;

	while (1) {
		while (empty_seats > 0) {
			pthread_mutex_lock(&mutex);
			if (empty_seats>0) {
				if ( queue>0 ) {
					passengers++;
					empty_seats--;
					queue--;
					pthread_cond_signal(&pass_sleep);
				}
				else{
					pthread_cond_wait(&start,&mutex);
				}
			}
			pthread_mutex_unlock(&mutex);
			if ((last == 1) && (queue <= 0)) {//an den exw allous pelates bgainw
				break;
			}
		}

		sleep(1);
		printf("Train start\n");
		sleep(arg->sleep_time );
		printf("Train finished\n");

		for ( i = 0; i < passengers; i++) {
			pthread_mutex_lock(&mutex);
			pthread_cond_signal(&finished);
			pthread_mutex_unlock(&mutex);
		}

		sleep(arg->sleep_time);
		pthread_mutex_lock(&mutex);
		empty_seats = MAX_SEATS;//arxikopoiw ksana tis metablites
		passengers = 0;
		printf("Train returned to station\n");
		pthread_mutex_unlock(&mutex);
		if ((last == 1) && (queue <= 0)) {//an den exw allous pelates bgainw
			break;
		}
	}
	return(0);

}
int main(int argc, char const *argv[]) {
	int rval;
	char c;
	int  sleep_time;
	args train_arg;
	pthread_t thread_train;
	pthread_t thread_pass;

	if (argc != 2){//if we have more arguments than the expected ones. we print
		//the error and we exit.
		printf("ERROR:wrong number of arguments \n");
		return 0;
	}
	printf("Enter passenger sequence with p and end with q for quit \n");
	sleep_time = atoi(argv[1]);

	train_arg.sleep_time = sleep_time;

	rval = pthread_create(&thread_train,NULL,train,(void *)&train_arg);
	if(rval){
		fprintf(stderr,"Error - pthread_create() return code: %d\n",rval);
		exit(EXIT_FAILURE);
	}

	while (1) {
		c = getchar();
		if (c == 'p') {
			rval=pthread_create(&thread_pass,NULL,passenger,NULL);
			if(rval){
				fprintf(stderr,"Error - pthread_create() return code: %d\n",rval);
			    exit(EXIT_FAILURE);
			}

			pthread_mutex_lock(&mutex);//mutex gia allagi tou global queue
			queue = queue + 1;
			printf("Thread %u joined the queue\n", (int)thread_pass);
			pthread_mutex_unlock(&mutex);
		}
		else if(c == 'q') {
			break;
		}
		sleep(1);
	}

	last = 1;
	pthread_mutex_lock(&mutex);
	if (queue == 0) {// an exei kolisei to trainaki kanoume ena up gia na ksekolisei kai
		// na katalabei oti teleiwsame giati mporei to q na erthei enw einai blockarismeno
		pthread_cond_signal(&start);
	}
	pthread_mutex_unlock(&mutex);
	pthread_join(thread_train ,NULL);

	return 0;
}
