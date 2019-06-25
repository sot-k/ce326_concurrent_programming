//#############################//
//# WRITTEN BY:               #//
//# CHOULIARAS ANDREAS   2143 #//
//# KARAMELLIOS SOTIRIOS 2237 #//
//#############################//
#include<unistd.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<sys/types.h>
#include "mythreads.h"
#include "lists.h"

#define INTERVAL 200000

#define  DEBUG

thr_t sched_thread, main_thread , reset;
volatile int thread_count=0;
thr_t *destroy=NULL;
int semnum=0;

static void handler_alarm(int signal){
	thr_t *running_q;
	//write(STDOUT_FILENO,"BEEP!\n",6);
	//context_switch();
	running_q = list_current();
	if(running_q->next==running_q){
	    return;
	}
	sched_thread.context.from = &(running_q->context);

	if(swapcontext(&(sched_thread.context.from->uctx),&(sched_thread.context.uctx))==-1)
		handle_error("coroutine switch(swap)");

	return;
}

void timer_start(int val){
	struct sigaction action_timer={{0}};
	struct itimerval timer={{0}};
	// Alarm, signal , signal_handler initialize
	//===SET TIMER
	timer.it_interval.tv_sec=0;
	timer.it_interval.tv_usec=val;
	timer.it_value.tv_sec=0;
	timer.it_value.tv_usec=val;

	//===SET TIMER HANDLER
	action_timer.sa_handler=handler_alarm;
	action_timer.sa_flags=SA_RESTART;
	sigaction(SIGALRM,&action_timer,NULL);

	setitimer(ITIMER_REAL,&timer,NULL);
	return;
}

void scheduler(void *arg){
	struct sigaction action_alarm={{0}};
	//===IGNORE SIGALRM
	action_alarm.sa_handler = SIG_IGN;
	action_alarm.sa_flags = SA_RESTART;
	sigaction(SIGALRM, &action_alarm, NULL);
	//timer_start(0);

	if (destroy!=NULL){
		mycoroutines_destroy(&(destroy->context));
		destroy=NULL;
	}

	context_switch();
}

void context_switch(){
	thr_t *from;
	thr_t *to;
	thr_t *curr, *temp;
	thr_t *running_q, *blocking_q;
	struct sigaction action_alarm={{0}};

	// #ifdef DEBUG
	// print_list();
	// #endif

	//===IGNORE SIGALRM
	action_alarm.sa_handler = SIG_IGN;
	action_alarm.sa_flags = SA_RESTART;
	sigaction(SIGALRM, &action_alarm, NULL);

	running_q = list_current();
	blocking_q=list_blocking();

	if(sched_thread.context.from != &(running_q->context) && (running_q == running_q->next)){
		curr=blocking_q;
			do{
				if(curr->wait != NULL){
					if(  &(curr->wait->context) == &(running_q->context) ){
						temp=remove_blocking(curr);
						temp->status=WAITING;
						list_add_tail(running_q,temp);
					}
				}
				curr=curr->next;
			}while(curr != blocking_q);
	}
	from=running_q;
	if (from->status == RUNNING) {
		from->status=WAITING;
	}

	to=running_q->next;
	to->status=RUNNING;
	//to->context.from=&(from->context);
	to->context.from=&(sched_thread.context);
	if (from->status == BLOCKED) {
		from=remove_running();
		//add to blocked list
		list_add_blocked(from);

	}
	if(sched_thread.context.from != &(running_q->context)){
		if (blocking_q!=NULL){
			curr=blocking_q;
			do{
				if(curr->wait != NULL){
					if(  &(curr->wait->context) == &(running_q->context) ){
						temp=remove_blocking(curr);
						temp->status=WAITING;
						list_add_tail(to,temp);
						blocking_q=list_blocking();
						if (blocking_q == NULL) {
							break;
						}
						curr = blocking_q;
					}
				}
				curr=curr->next;
			}while(curr != blocking_q);
		}
		destroy=remove_running();
		if (destroy->tid == 1) {
			destroy=NULL;
		}
		else{
			#ifdef DEBUG
			printf("Remove node %d\n", destroy->tid );
			#endif
		}
		sched_thread.context.from=NULL;
	}
	list_head(to);

	#ifdef DEBUG
	//print_list();
	printf("switching from %d to %d\n", from->tid, to->tid);
	#endif

	timer_start(INTERVAL);

	mycoroutines_jump( &(to->context));
	return;
}

// initialize api
int mythreads_init(){
	char *thr_stack=NULL;
	//struct sigaction action_timer={{0}};
	//struct itimerval timer={{0}};

	// scheduler initialize
	sched_thread.tid=thread_count;
	thread_count++;
	sched_thread.status=0;
	sched_thread.sem_lock=NULL;
	thr_stack=(char *)malloc(STACK_SIZE);
	sched_thread.context.stack=thr_stack;
	sched_thread.context.size_stack=STACK_SIZE;
	sched_thread.context.link=NULL;
	mycoroutines_create(&(sched_thread.context),scheduler,NULL);
	sched_thread.wait=NULL;

	//main initialize
	//main_thread=(thr_t *)malloc(sizeof(thr_t));
	mycoroutines_init(&(main_thread.context));
	main_thread.tid=thread_count;
	thread_count++;
	main_thread.status=RUNNING;
	main_thread.sem_lock=NULL;
	main_thread.wait=NULL;
	main_thread.next=&main_thread;
	main_thread.prev=&main_thread;
	// the queue is empty so we put main as the only main_thread inside
	list_head(&main_thread);

	timer_start(INTERVAL);

	return 0;
}

int mythreads_create(thr_t *thr, void (body)(void *), void *arg){
	thr_t *running_q;
	char *thr_stack=NULL;
	sigset_t block_signal;

	sigemptyset(&block_signal);
    sigaddset(&block_signal, SIGALRM);
    sigprocmask(SIG_BLOCK, &block_signal, NULL);


	thr->tid=thread_count;
	thread_count++;
	thr->status=WAITING;
	thr->sem_lock=NULL;
	thr->wait=NULL;
	thr_stack=(char *)malloc(STACK_SIZE);
	thr->context.stack=thr_stack;
	thr->context.size_stack=STACK_SIZE;
	thr->context.link=&(sched_thread.context);
	mycoroutines_create(&(thr->context),body,arg);
	running_q = list_current();
	list_add_tail(running_q,thr);

	sigprocmask(SIG_UNBLOCK, &block_signal, NULL);

	return 0;
}

int mythreads_yield(){
	thr_t *running_q;
	sigset_t block_signal;

	sigemptyset(&block_signal);
    sigaddset(&block_signal, SIGALRM);
    sigprocmask(SIG_BLOCK, &block_signal, NULL);

	running_q = list_current();
	if(running_q->next==running_q){
		sigprocmask(SIG_UNBLOCK, &block_signal, NULL);
	    return 0;
	}

	sched_thread.context.from = &(running_q->context);

	if(swapcontext(&(sched_thread.context.from->uctx),&(sched_thread.context.uctx))==-1)
		handle_error("coroutine switch(swap)");
	//printf("I AM HERE\n");
	return 0;
}

int mythreads_join(thr_t *thr){
	thr_t *running_q;
	sigset_t block_signal;

	sigemptyset(&block_signal);
    sigaddset(&block_signal, SIGALRM);
    sigprocmask(SIG_BLOCK, &block_signal, NULL);

	running_q = list_current();

	if (list_find(thr->tid) != NULL){
		running_q->wait=thr;
		//printf("I found 1\n");
	}
	else{
		//printf("I AM HERE\n");
		sigprocmask(SIG_UNBLOCK, &block_signal, NULL);
		return 0;
	}
	running_q->status = BLOCKED;
	//print_list();
	sched_thread.context.from = &(running_q->context);
	mycoroutines_switchto(&(sched_thread.context));
	return 0;

}

int mythreads_destroy(thr_t *thr){
	//sigset_t block_signal;

	if (thr == NULL) {
		return 0;
	}
	//printf("I AM HERE %d\n", thr->tid );
	free(thr);
	thr=NULL;

	return 0;
}

int get_semid(){
	int semid;
	semid=semnum;
	semnum++;
	return semid;
}

/////////////////////SEMAPHORE FUNCTIONS STARTS HERE//////////////////////////

int mythreads_sem_init(sem_t *s,int val){

	s->semid=get_semid();
	s->semval=val;

	return 0;
}

int mythreads_sem_down(sem_t *s){
	sigset_t block_signal;

	sigemptyset(&block_signal);
    sigaddset(&block_signal, SIGALRM);
    sigprocmask(SIG_BLOCK, &block_signal, NULL);
	if (s->semval > 0) {
		s->semval--;
	} else {
		thr_t *running_q;
		/* put thread in block queue */
		running_q=list_current();
		running_q->status=BLOCKED;
		running_q->sem_lock=s;
		sched_thread.context.from = &(running_q->context);
		mycoroutines_switchto(&(sched_thread.context));
	}
	sigprocmask(SIG_UNBLOCK, &block_signal, NULL);
	return 0;
}

int mythreads_sem_up(sem_t *s){
	thr_t *running_q, *node=NULL;
	sigset_t block_signal;

	sigemptyset(&block_signal);
    sigaddset(&block_signal, SIGALRM);
    sigprocmask(SIG_BLOCK, &block_signal, NULL);
	if (s->semval > 0) {
		s->semval++;
	} else {
		//blocking_q=list_blocking();
		node=list_blocked_find(s->semid);
		if (node == NULL){
			s->semval++;
			sigprocmask(SIG_UNBLOCK, &block_signal, NULL);
			return 0;
		}
		node=remove_blocking(node);
		node->status=WAITING;
		node->sem_lock=NULL;
		running_q=list_current();
		list_add_tail(running_q,node);
		//print_list();
	}
	sigprocmask(SIG_UNBLOCK, &block_signal, NULL);
	return 0;
}

int mythreads_sem_destroy(sem_t *s){
	//printf("I AM HERE 1\n");
	free(s);
	return 0;
}
