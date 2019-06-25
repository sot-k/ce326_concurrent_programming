//#############################//
//# WRITTEN BY:               #//
//# CHOULIARAS ANDREAS   2143 #//
//# KARAMELLIOS SOTIRIOS 2237 #//
//#############################//

#ifndef COROUTINES
#define COROUTINES

#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>

#define handle_error(msg) \
   do { perror(msg); exit(EXIT_FAILURE); } while (0);

#define STACK_SIZE 16384

typedef struct co_data{
	ucontext_t uctx;
	char *stack;
	int size_stack;
	struct co_data *from;
	struct co_data *link;
} co_t;

int mycoroutines_init(co_t *main);

int mycoroutines_create(co_t *co, void (body)(void *),void *arg);

int mycoroutines_switchto(co_t *co);

int mycoroutines_jump(co_t *co);

int mycoroutines_destroy(co_t *co);

#endif
