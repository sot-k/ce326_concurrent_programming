//#############################//
//# WRITTEN BY:               #//
//# CHOULIARAS ANDREAS   2143 #//
//# KARAMELLIOS SOTIRIOS 2237 #//
//#############################//
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
// struct sembuf
// {
//   unsigned short int sem_num;	/* semaphore number */
//   short int sem_op;		/* semaphore operation */
//   short int sem_flg;		/* operation flag */
// };
union semun
{
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO
                           	(Linux-specific) */
};

int mysem_create(int nsems,unsigned short init_vals[]) {
	key_t key;
    int semid;
	char path[LINE_MAX];
	union semun arg;

	getcwd(path, sizeof(path));
	key = ftok(path, 'l');
	printf("%d\n",key );

	/* create a semaphore set with n semaphores: */
	if ((semid = semget(key, nsems, 0666 | IPC_CREAT)) == -1) {
		perror("semget");
		exit(1);
	}
	arg.array = init_vals;
	if (semctl(semid, 0, SETALL, arg) == -1) {
		perror("semctl");
		exit(1);
	}
	return semid;
}

int mysem_down(int semid, int semnumber) {
	struct sembuf operation;

	operation.sem_num = semnumber;
	operation.sem_op = -1;
	operation.sem_flg = 0;
	/*Decrease semaphore value by 1 *** Possible lock ***/
	if (semop(semid, &operation, 1) == -1) {
		perror("semop");
		exit(1);
	}
	return 0;
}

int mysem_up(int semid, int semnumber) {
	struct sembuf operation;
	int curr_val=0;

	operation.sem_num = semnumber;
	operation.sem_op = 1;
	operation.sem_flg = 0;

	curr_val = semctl(semid,semnumber,GETVAL,0);
	/*Error check of binary semaphore to not exceed value 1: */
	if (curr_val==1) {
		printf("Trying to raise value of a semaphore with an already value of 1");
		return EAGAIN;
	}
	/* Raise semaphore value by 1:*/
	if (semop(semid, &operation, 1) == -1) {
		perror("semop");
		exit(1);
	}
	return 0;
}

int mysem_destroy(int semid) {
	union semun arg;

	/* remove semaphore set: */
	if (semctl(semid, 0, IPC_RMID, arg) == -1) {
		perror("semctl");
		exit(1);
	}
	return 0;
}
