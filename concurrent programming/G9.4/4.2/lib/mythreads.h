//#############################//
//# WRITTEN BY:               #//
//# CHOULIARAS ANDREAS   2143 #//
//# KARAMELLIOS SOTIRIOS 2237 #//
//#############################//

#ifndef MYTHREADS
#define MYTHREADS

#include "coroutines.h"

#define WAITING 'w'
#define RUNNING 'r'
#define BLOCKED 'b'

typedef struct semaphore{
   int semid;
   int semval;
} sem_t;

typedef struct thread_node {
		int tid;
		co_t context;
		char status;
		sem_t *sem_lock;
		struct thread_node *next;
		struct thread_node *prev;
		struct thread_node *wait;
} thr_t;

void context_switch();

int mythreads_init(); // initialize api

int mythreads_create(thr_t *thr, void (body)(void *), void *arg);

int mythreads_yield();

int mythreads_join(thr_t *thr);

int mythreads_destroy(thr_t *thr);

int mythreads_sem_init(sem_t *s,int val);

int mythreads_sem_down(sem_t *s);

int mythreads_sem_up(sem_t *s);

int mythreads_sem_destroy(sem_t *s);

#endif
