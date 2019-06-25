//#############################//
//# WRITTEN BY:               #//
//# CHOULIARAS ANDREAS   2143 #//
//# KARAMELLIOS SOTIRIOS 2237 #//
//#############################//

#include "lists.h"

thr_t *running_q=NULL;
thr_t *blocking_q=NULL;

// check if list is empty
int list_is_empty(thr_t *head){
	return (head==NULL)? 1:0;
}

// returns the head of the running queue
thr_t *list_current(){
	return running_q;
}

thr_t *list_blocking(){
	return blocking_q;
}

// returns the node after the specified head
thr_t *list_next(thr_t *head){
	return head->next;
}

// returns the node before the specified head
thr_t *list_prev(thr_t *head){
	return head->prev;
}

// sets the head of the running queue the the specified head
int list_head(thr_t *head){
	running_q=head;
	return 0;
}

//Insert node after the specified head.
int list_add(thr_t *head,thr_t *new){
	if(head==NULL){
		head=new;
		return 0;
	}
	new->prev=head;
	new->next=head->next;
	head->next=new;
	new->next->prev=new;
	return 0;
}

// Insert node before the specified head
int list_add_tail(thr_t *head,thr_t *new){
	if(head==NULL){
		head=new;
		return 0;
	}
	new->next=head;
	new->prev=head->prev;
	head->prev=new;
	new->prev->next=new;
	return 0;
}

//Insert node in blocked queue tail
int list_add_blocked(thr_t *new){
	if(blocking_q==NULL){
		blocking_q=new;
		return 0;
	}
	new->next=blocking_q;
	new->prev=blocking_q->prev;
	blocking_q->prev=new;
	new->prev->next=new;

	//blocking_q=blocking_q->next;
	return 0;
}

// find the node with the given tid
thr_t *list_find(int tid){
	thr_t *curr=NULL;
	curr = running_q;
	do {
		if(curr->tid == tid )
			break;
		curr = curr->next;
	} while(curr != running_q);
	if (curr==running_q){
		return NULL;
	}
	return curr;
}

// find the node blocked by the given semid
thr_t *list_blocked_find(int semid){
	thr_t *curr=NULL;

	curr = blocking_q;
	if (curr==NULL) {
		return NULL;
	}
	do {
		if (curr->sem_lock !=NULL){
			if(curr->sem_lock->semid == semid )
				return curr;
		}
		curr = curr->next;
	} while(curr != blocking_q);
	if (curr==blocking_q){
		return NULL;
	}
	return NULL;
}

// remove first node from running queue.It returns the removed node
thr_t *remove_running(){
	thr_t *old;
	old=running_q;
	if (running_q->next==running_q){
		running_q=NULL;
		return old;
	}
	running_q=running_q->next;
	old=list_remove_tail(running_q);
	return old;
}

// remove node from blocking list specified by head.It returns the removed node
thr_t *remove_blocking(thr_t *head){
	thr_t *rval;
	rval=head;
	if (head->next==head){
		blocking_q=NULL;
		return rval;
	}
	if (head==blocking_q) {
		blocking_q=blocking_q->next;
	}
	head=head->next;
	rval=list_remove_tail(head);
	return rval;
}

// remove node before specified head.It returns the removed node
thr_t *list_remove_tail(thr_t *head){
	thr_t *old;
	old=head;
	if (head->next==head){
		head=NULL;
		return old;
	}
	old=head->prev;
	head->prev=old->prev;
	old->prev->next=head;
	old->next=old->prev=old;
	return old;
}

void print_list(){
	thr_t *curr=NULL;
	// for (curr = running_q; curr != running_q ; curr=curr->next ) {
	// 	printf("%d\n", curr->tid);
	// }
	curr = running_q;
	printf("=ready queue\n");
	do {
		printf("%d %c\n", curr->tid, curr->status);
		curr = curr->next;
	} while(curr != running_q);

	printf("=blocking queue\n");
	curr = blocking_q;
	if (curr==NULL) {
		printf("empty list\n");
		printf("=end\n");
		return;
	}
	do {
		printf("=%d %c\n", curr->tid, curr->status);
		curr = curr->next;
	} while(curr != blocking_q);
	printf("=end\n");
}
