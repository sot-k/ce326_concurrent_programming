#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int *table;

typedef struct arguments{
	int busy;		//Flag that informs main if a worker is busy or not
	int high;		//high border of the sub array to be sorted
	int low;		//low border of the sub array to be sorted
} args;



void *sorting(void *input) {

	args *arg;
	arg = (args *) input;
	int pivot, i, j, temp, rval;
	args *arguments_low, *arguments_high;
	pthread_t worker_low, worker_high;


  	if(arg->low < arg->high) {
    	pivot = arg->low; // select a pivot element
    	i = arg->low;
    	j = arg->high;
    	while(i < j) {
      	// increment i till you get a number greater than the pivot element
      		while(table[i] <= table[pivot] && i <= arg->high){
				i++;
			}
      		// decrement j till you get a number less than the pivot element
      		while(table[j] > table[pivot] && j >= arg->low){
				j--;
			}
      	// if i < j swap the elements in locations i and j
      		if(i < j) {
        		temp = table[i];
        		table[i] = table[j];
        		table[j] = temp;
      		}
    	}

    // when i >= j it means the j-th position is the correct position
    // of the pivot element, hence swap the pivot element with the
    // element in the j-th position
    	temp = table[j];
    	table[j] = table[pivot];
    	table[pivot] = temp;
    // Repeat quicksort for the two sub-arrays, one to the left of j
    // and one to the right of j

	// quicksort(table, arg->low, j-1)
		arguments_low = (args *)malloc(sizeof(args));
		arguments_low->busy = 1;
		arguments_low->low = arg->low;
		arguments_low->high = j - 1;
		rval = pthread_create(&worker_low,NULL,sorting,(void *) arguments_low);
		if(rval){
			fprintf(stderr,"Error worker_low - pthread_create() return code: %d\n",rval);
			exit(EXIT_FAILURE);
		}

		// quicksort(table, j+1, arg->high)
		arguments_high = (args *)malloc(sizeof(args));
		arguments_high->busy = 1;
		arguments_high->low = j + 1;
		arguments_high->high = arg->high;
		rval = pthread_create(&worker_high,NULL,sorting,(void *) arguments_high);
		if(rval){
			fprintf(stderr,"Error worker_high - pthread_create() return code: %d\n",rval);
			exit(EXIT_FAILURE);
		}

//waiting for both threads to finish
		while (arguments_low->busy || arguments_high->busy) {

		}
		free(arguments_low);
		free(arguments_high);

  	}

//notifying the master thread(or the main function) that you have finished
//working and waiting for your child threads
	arg->busy = 0;
	return 0;
}




int main(int argc, char const *argv[]) {

	int ret, integer, size, rval;
	pthread_t worker;
	args arguments;
	size = 0;

	table = (int *)malloc(sizeof(int));//initialising a table of a size of 1 int
	ret = scanf(" %d\n",&integer ); //scaning for the first integer
	while(ret == 1){//while the scaning is valid
		size = size + 1;
		//using realloc to make the array 1 spot larger
		table = (int *)realloc(table, size * sizeof(int));
		table[size - 1] = integer;
		ret = scanf(" %d\n",&integer );//scan for the next
	}

//initialising the master thread
	arguments.busy = 1;
	arguments.low = 0;
	arguments.high = size - 1;
	rval = pthread_create(&worker,NULL,sorting,(void *) &arguments);
	if(rval){
		fprintf(stderr,"Error - pthread_create() return code: %d\n",rval);
		exit(EXIT_FAILURE);
	}

//waiting for the master thread
	while (arguments.busy) {

	}

//printing the table results
	for (size_t i = 0; i < size; i++) {
		printf("%d\n",table[i] );
	}

//free the table and return.
	free(table);

	return 0;
}
