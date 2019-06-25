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
#include "my_sem.h"


#define BUSY 0
#define GOTNUM 1
#define SLEEP 2

typedef struct arguments{
	int *number ;	//Number that worker tests if prime
	int semid ;		//Flag that informs main if a worker is busy or not
	int die ;		//flag for worker to stop working and close
} args;//data structure to be passed as thread arguments.


/*Worker thread :calculates if input number is prime
 *Uses busy waiting until main gives work to the thread.
 *if main "signals" threads to close ,it closes if it is not working
 *or finishes the work first and then it closes.Anyway before closing it informs
 *main that it dies*/
void *slave(void *input){

	args *arg;
	arg = (args *) input;
	int num;
	printf("Work start from Slave:%u\n", (int)pthread_self() );
	mysem_down(arg->semid, BUSY);
	while ((arg->die==0)) { 			//wait until work arrives or until i get close signal
		num = *(arg->number);       //If work arrived for me
		mysem_up(arg->semid, GOTNUM);
		if (prime_test(num)) {				// Stark working
			printf("Work Done by slave %u: %d is prime \n",(int)pthread_self(),num );
		}
		else{
			printf("Work Done by slave %u: %d is NOT prime \n",(int)pthread_self(),num );
		}
		mysem_down(arg->semid, BUSY);	//work done i am available again for another one
	}
									//Close
	mysem_up(arg->semid, SLEEP);								//Inform main i closed
	printf("Work finish by Slave:%u ...Going to Sleep\n", (int)pthread_self() );
	return 0;
}
/*Main thread.It creates N threads and initialises its values.Then it read numbers from stdin
until 0 is readed.For every number it reads ,it finds the first available worker and it assigns the "work"
to that thread.if none worker is available main waits (busy waiting) until one becomes available again.
When all workload ends it informs workers to close and waits until all of them closes sucessfully.Then it also closes */
int main(int argc, char const *argv[]) {
	int n,rval,checknum;
	int semid;
	short unsigned seminit[] = {0,0,0};
	args *array_ptr;
	pthread_t *thread_array;

	if (argc != 2){//if we have more arguments than the expected ones. we print
		//the error and we exit.
		printf("ERROR:wrong number of arguments \n");
		return 0;
	}

	semid = mysem_create(3, seminit);

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
	for (size_t i = 0; i < n; i++) {	//CREATE N THREADS and initialises them
		(array_ptr+i)->number= &checknum;
		(array_ptr+i)->semid = semid;
		(array_ptr+i)->die=0;
		rval=pthread_create(thread_array+i,NULL,slave,(void *) (array_ptr+i));
		if(rval){
			fprintf(stderr,"Error - pthread_create() return code: %d\n",rval);
		    exit(EXIT_FAILURE);
		}
	}
	scanf(" %d\n", &checknum );
	while (checknum != 0) { 		//Assign workload to slaves
		mysem_up(semid, BUSY);
		mysem_down(semid, GOTNUM);
		scanf(" %d\n", &checknum ); //NEW work arrives that needs to be assigned again
	}
	for (size_t i = 0; i < n ; i++) {	// Tell slaves work finished and they must close
		(array_ptr+i)->die=1;
	}

	for (size_t i = 0; i < n; i++) {
		mysem_up(semid, BUSY);
		mysem_down(semid, SLEEP);
	}

	free(thread_array);
	free(array_ptr);

	mysem_destroy(semid);

	// for (size_t i = 0; i < n; i++) {	//Join N THREADS
	// 	pthread_join(*(thread_array+i),NULL);
	// }

	return 0;
}
