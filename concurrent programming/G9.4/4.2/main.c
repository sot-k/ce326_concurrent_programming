//#############################//
//# WRITTEN BY:               #//
//# CHOULIARAS ANDREAS   2143 #//
//# KARAMELLIOS SOTIRIOS 2237 #//
//#############################//

#include <stdio.h>
#include <stdlib.h>
#include "lib/mythreads.h"
#include "lib/coroutines.h"

thr_t *func11,*func21,*func31,*func41,*func51;
sem_t *mutex,*mutex2;

void func2(void *input) {
	printf("HELLO\n");

	mythreads_sem_down(mutex2);
	for (size_t i = 0; i < 10000; i++) {
		//printf("%s\n", );
	}
	return;
}
void func3(void *input) {
	printf("FROM\n");

	mythreads_sem_up(mutex);
	for (size_t i = 0; i < 10000; i++) {
		//printf("%s\n", );
	}
	return;
}
void func4(void *input) {
	printf("THE\n");
	mythreads_sem_up(mutex2);
	mythreads_yield();
	for (size_t i = 0; i < 10000; i++) {
		//printf("%s\n", );
	}
	return;
}
void func5(void *input) {
	printf("OTHER\n");
	mythreads_sem_down(mutex);
	for (size_t i = 0; i < 10000; i++) {
		//printf("%s\n", );
	}
	mythreads_yield();
	return;
}
void func6(void *input) {
	printf("SIDE\n");
	mythreads_sem_up(mutex);
	for (size_t i = 0; i < 10000; i++) {
		//printf("%s\n", );
	}
	return;
}

int main(int argc, char const *argv[]) {

	mythreads_init();
	mutex = (sem_t *)malloc(sizeof(sem_t));
	mutex2 = (sem_t *)malloc(sizeof(sem_t));
	mythreads_sem_init(mutex,0);
	mythreads_sem_init(mutex2,0);
	printf("INIT DONE\n");

	func11 = (thr_t *)malloc(sizeof(thr_t));
	func21 = (thr_t *)malloc(sizeof(thr_t));
	func31 = (thr_t *)malloc(sizeof(thr_t));
	func41 = (thr_t *)malloc(sizeof(thr_t));
	func51 = (thr_t *)malloc(sizeof(thr_t));
	mythreads_create(func11,func2,NULL);
	mythreads_create(func21,func3,NULL);
	mythreads_create(func31,func4,NULL);
	mythreads_create(func41,func5,NULL);
	mythreads_create(func51,func6,NULL);

	// for (size_t i = 0; i < 10000; i++) {
	// 	//printf("%s\n", );
	// }

	mythreads_sem_down(mutex);
	mythreads_join(func11);
	printf("GOODBYE 2\n");
	mythreads_join(func21);
	printf("GOODBYE 3\n");
	mythreads_join(func31);
	printf("GOODBYE 4\n");
	mythreads_join(func41);
	printf("GOODBYE 5\n");
	mythreads_join(func51);
	printf("GOODBYE 6\n");
	mythreads_yield();
	// does not print that
	printf("GOODBYE\n");
	mythreads_sem_destroy(mutex);
	return 0;
}
