//#############################//
//# WRITTEN BY:               #//
//# CHOULIARAS ANDREAS   2143 #//
//# KARAMELLIOS SOTIRIOS 2237 #//
//#############################//

#ifndef LISTS
#define LISTS

#include "mythreads.h"

// check if list is empty
int list_is_empty(thr_t *head);

// returns the head of the running queue
thr_t *list_current();

thr_t *list_blocking();

// returns the node after the specified head
thr_t *list_next(thr_t *head);

// returns the node before the specified head
thr_t *list_prev(thr_t *head);

// sets the head of the running queue the the specified head
int list_head(thr_t *head);

//Insert node after the specified head.
int list_add(thr_t *head,thr_t *new);

//Insert node in blocked queue tail
int list_add_blocked(thr_t *new);

// Insert node before the specified head
int list_add_tail(thr_t *head,thr_t *new);

// find the node with the given tid
thr_t *list_find(int tid);

// find the node blocked by the given semid
thr_t *list_blocked_find(int tid);

// remove node after specified head.It returns the removed node
thr_t *remove_running();

// remove node from blocking list specified by head.It returns the removed node
thr_t *remove_blocking(thr_t *head);

// remove node before specified head.It returns the removed node
thr_t *list_remove_tail(thr_t *head);

void print_list();

#endif
