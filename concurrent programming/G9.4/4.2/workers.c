//#############################//
//# WRITTEN BY:               #//
//# CHOULIARAS ANDREAS   2143 #//
//# KARAMELLIOS SOTIRIOS 2237 #//
//#############################//

#include<stdio.h>
#include<malloc.h>
#include<memory.h>
#include<stdlib.h>
#include<error.h>
#include<unistd.h>
#include "prime_test.h"
#include "lib/mythreads.h"
#include "lib/coroutines.h"


sem_t *busy,*gotnum,*sleeps;

typedef struct arguments{
	int *number ;	//Number that worker tests if prime
	thr_t *self ;		//Flag that informs main if a worker is busy or not
	int die ;		//flag for worker to stop working and close
} args;//data structure to be passed as thread arguments.


/*Worker thread :calculates if input number is prime
 *Uses busy waiting until main gives work to the thread.
 *if main "signals" threads to close ,it closes if it is not working
 *or finishes the work first and then it closes.Anyway before closing it informs
 *main that it dies*/
void slave(void *input){

	args *arg;
	arg = (args *) input;
	int num;
	printf("Work start from Slave:%u\n", (int)arg->self->tid );
	mythreads_sem_down(busy);
	while ((arg->die==0)) { 			//wait until work arrives or until i get close signal
		num = *(arg->number);       //If work arrived for me
		mythreads_sem_up(gotnum);
		if (prime_test(num)) {				// Stark working
			printf("Work Done by slave %u: %d is prime \n",(int)arg->self->tid,num );
		}
		else{
			printf("Work Done by slave %u: %d is NOT prime \n",(int)arg->self->tid,num );
		}
		mythreads_sem_down(busy);	//work done i am available again for another one
	}
									//Close
	mythreads_sem_up(sleeps);								//Inform main i closed
	printf("Work finish by Slave:%u ...Going to Sleep\n", (int)arg->self->tid );
	return;
}
/*Main thread.It creates N threads and initialises its values.Then it read numbers from stdin
until 0 is readed.For every number it reads ,it finds the first available worker and it assigns the "work"
to that thread.if none worker is available main waits (busy waiting) until one becomes available again.
When all workload ends it informs workers to close and waits until all of them closes sucessfully.Then it also closes */
int main(int argc, char const *argv[]) {
	int n,rval,checknum,i;
	args *array_ptr;
	thr_t *thread_array;

	if (argc != 2){//if we have more arguments than the expected ones. we print
		//the error and we exit.
		printf("ERROR:wrong number of arguments \n");
		return 0;
	}

	mythreads_init();
	busy = (sem_t *)malloc(sizeof(sem_t));
	gotnum = (sem_t *)malloc(sizeof(sem_t));
	sleeps = (sem_t *)malloc(sizeof(sem_t));
	mythreads_sem_init(busy,0);
	mythreads_sem_init(gotnum,0);
	mythreads_sem_init(sleeps,0);
	printf("INIT DONE\n");

	n=atoi(argv[1]);
	thread_array= (thr_t *)malloc(n*sizeof( thr_t ));
	if (thread_array == NULL) {
		fprintf(stderr,"Error - malloc space allocation failure\n");
		exit(EXIT_FAILURE);
	}

	array_ptr= (args *)malloc(n*sizeof( args ));
	if (array_ptr == NULL) {
		fprintf(stderr,"Error2 - malloc space allocation failure\n");
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < n; i++) {	//CREATE N THREADS and initialises them
		(array_ptr+i)->number= &checknum;
		(array_ptr+i)->self = thread_array+i;
		(array_ptr+i)->die=0;
		rval=mythreads_create(thread_array+i,slave,(void *) (array_ptr+i));
		if(rval){
			fprintf(stderr,"Error - pthread_create() return code: %d\n",rval);
		    exit(EXIT_FAILURE);
		}
	}
	scanf(" %d\n", &checknum );
	while (checknum != 0) { 		//Assign workload to slaves
		mythreads_sem_up(busy);
		mythreads_sem_down(gotnum);
		scanf(" %d\n", &checknum ); //NEW work arrives that needs to be assigned again
	}
	for (i = 0; i < n ; i++) {	// Tell slaves work finished and they must close
		(array_ptr+i)->die=1;
	}

	for (i = 0; i < n; i++) {
		mythreads_sem_up(busy);
		//mythreads_sem_down(semid, SLEEP);
		mythreads_join(thread_array+i);
	}

	mythreads_destroy(thread_array);
	free(array_ptr);

	mythreads_sem_destroy(busy);
	mythreads_sem_destroy(gotnum);
	mythreads_sem_destroy(sleeps);
	// for (size_t i = 0; i < n; i++) {	//Join N THREADS
	// 	pthread_join(*(thread_array+i),NULL);
	// }
	return 0;
}
