//#############################//
//# WRITTEN BY:               #//
//# CHOULIARAS ANDREAS   2143 #//
//# KARAMELLIOS SOTIRIOS 2237 #//
//#############################//

#ifndef CCR
#define CCR
#include <pthread.h>

#define CCR_DECLARE(label) pthread_mutex_t label##_mutex= PTHREAD_MUTEX_INITIALIZER;\
	pthread_mutex_t label##_mtx_S= PTHREAD_MUTEX_INITIALIZER;\
	pthread_cond_t label##_Q1 = PTHREAD_COND_INITIALIZER;\
	pthread_cond_t label##_Q2 = PTHREAD_COND_INITIALIZER;\
	unsigned int label##_N1;\
	unsigned int label##_N2;

#define CCR_INIT(label) label##_N1=0;\
	label##_N2=0;

#define CCR_EXEC(label,cond,body) pthread_mutex_lock(&label##_mutex);\
			while(!cond){											 \
				label##_N1++;										 \
				if (label##_N2>0) {									 \
					label##_N2--;									 \
					pthread_cond_signal(&label##_Q2);				 \
				}													 \
				pthread_cond_wait(&label##_Q1,&label##_mutex);		 \
				label##_N2++;										 \
				if (label##_N1>0) {									 \
					label##_N1--;									 \
					pthread_cond_signal(&label##_Q1);				 \
				} else if(label##_N2 > 1){							 \
					label##_N2--;									 \
					pthread_cond_signal(&label##_Q2);				 \
					pthread_cond_wait(&label##_Q2,&label##_mutex);	 \
				}													 \
				else{												 \
					label##_N2--;									 \
				}													 \
			}														 \
			body													 \
			if (label##_N1>0) {										 \
				label##_N1--;										 \
				pthread_cond_signal(&label##_Q1);					 \
			} else if (label##_N2>0) {								 \
				label##_N2--;										 \
				pthread_cond_signal(&label##_Q2);					 \
			}														 \
			pthread_mutex_unlock(&label##_mutex);

#endif
