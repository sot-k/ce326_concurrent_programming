//#############################//
//# WRITTEN BY:               #//
//# CHOULIARAS ANDREAS   2143 #//
//# KARAMELLIOS SOTIRIOS 2237 #//
//#############################//
#ifndef MYSEM
#define MYSEM

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include "my_sem.h"


int mysem_create(int nsems,unsigned short init_vals[]);

int mysem_down(int semid, int semnumber);

int mysem_up(int semid, int semnumber);

int mysem_destroy(int semid);

#endif
