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
#include "prime_test.h"

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t taken=PTHREAD_COND_INITIALIZER;
pthread_cond_t avail=PTHREAD_COND_INITIALIZER;
pthread_cond_t reply=PTHREAD_COND_INITIALIZER;
int worker_free=0;
int num_taken=1;

typedef struct arguments{
	int *number ;	//Number that worker tests if prime
	int die ;		//flag for worker to stop working and close
} args;//data structure to be passed as thread arguments.

/*Worker thread :calculates if input number is prime
 *Uses   waiting until main gives work to the thread.
 *if main "signals" threads to close ,it closes if it is not working
 *or finishes the work first and then it closes.Anyway before closing it informs
 *main that it dies*/
void *slave(void *input){

	args *arg;
	arg = (args *) input;
	int num;
	printf("Work start from Slave:%u\n", (int)pthread_self() );
	while ((arg->die==0)) { 			//wait until work arrives or until i get close signal

		pthread_mutex_lock(&mutex);
		//worker_free++;
		while ( num_taken==1 )  {
			pthread_cond_wait(&avail,&mutex);
			if (arg->die==1) {
				printf("Work finish by Slave:%u ...Going to Sleep\n", (int)pthread_self() );
				pthread_mutex_lock(&mutex);
				pthread_cond_signal(&reply);
				pthread_mutex_unlock(&mutex);
				return 0;
			}
		}
		num = *(arg->number);       //If work arrived for me
		num_taken=1;
		//worker_free--;
		pthread_cond_signal(&taken);
		pthread_mutex_unlock(&mutex);

		if (prime_test(num)) {				// Stark working
			printf("Work Done by slave %u: %d is prime \n",(int)pthread_self(),num );
		}
		else{
			printf("Work Done by slave %u: %d is NOT prime \n",(int)pthread_self(),num );
		}
	}
	printf("Work finish by Slave:%u ...Going to Sleep\n", (int)pthread_self() );
	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&reply);
	pthread_mutex_unlock(&mutex);
	return 0;
}
/*Main thread.It creates N threads and initialises its values.Then it read numbers from stdin
until 0 is readed.For every number it reads ,it finds the first available worker and it assigns the "work"
to that thread.if none worker is available main waits until one becomes available again.
When all workload ends it informs workers to close and waits until all of them closes sucessfully.Then it also closes */
int main(int argc, char const *argv[]) {
	int n,rval,checknum,workNum;
	int i;
	args *array_ptr;
	pthread_t *thread_array;

	if (argc != 2){//if we have more arguments than the expected ones. we print
		//the error and we exit.
		printf("ERROR:wrong number of arguments \n");
		return 0;
	}

	n=atoi(argv[1]);
	thread_array= (pthread_t *)malloc(n*sizeof( pthread_t ));
	if (thread_array == NULL) {
		fprintf(stderr,"Error - malloc space allocation failure\n");
		exit(EXIT_FAILURE);
	}

	array_ptr= (args *)malloc(n*sizeof( args ));
	if (array_ptr == NULL) {
		fprintf(stderr,"Error2 - malloc space allocation failure\n");
		exit(EXIT_FAILURE);
	}
	for ( i = 0; i < n; i++) {	//CREATE N THREADS and initialises them
		(array_ptr+i)->number= &checknum;
		(array_ptr+i)->die=0;
		rval=pthread_create(thread_array+i,NULL,slave,(void *) (array_ptr+i));
		if(rval){
			fprintf(stderr,"Error - pthread_create() return code: %d\n",rval);
		    exit(EXIT_FAILURE);
		}
	}
	scanf(" %d\n", &workNum );
	while (workNum != 0) { 		//Assign workload to slaves

		pthread_mutex_lock(&mutex); //PUT
		if ( num_taken==0 ) pthread_cond_wait(&taken,&mutex);
		checknum=workNum;
		num_taken=0;
		//if (worker_free>0) {
			pthread_cond_signal(&avail);
		//}
		pthread_mutex_unlock(&mutex);

		scanf(" %d\n", &workNum ); //NEW work arrives that needs to be assigned again
	}
	for ( i = 0; i < n ; i++) {	// Tell slaves work finished and they must close
		(array_ptr+i)->die=1;
	}

	for ( i = 0; i < n; i++) {
		pthread_mutex_lock(&mutex);
		pthread_cond_signal(&avail);
		pthread_cond_wait(&reply,&mutex);
		pthread_mutex_unlock(&mutex);
		//pthread_join(*(thread_array+i),NULL);
	}

	free(thread_array);
	free(array_ptr);

	return 0;
}
