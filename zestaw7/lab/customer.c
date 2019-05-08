#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define MAX 10

static struct sembuf buf;

void podnies(int semid, int semnum){
  buf.sem_num = semnum;
  buf.sem_op = 1;
  buf.sem_flg = 0;
  if (semop(semid, &buf, 1) == -1){
    perror("Podnoszenie semafora");
    exit(1);
  }
}

void opusc(int semid, int semnum){
  buf.sem_num = semnum;
  buf.sem_op = -1;
  buf.sem_flg = 0;
  if (semop(semid, &buf, 1) == -1){
    perror("Opuszczenie semafora");
    exit(1);
  }
}

int main(){
 	int shmid, semid, i;
	int *buf;

 	semid = semget(45281, 2, IPC_CREAT|0600);

	if (semid == -1){
 		exit(1);
	}

	if (semctl(semid, 0, SETVAL, (int)MAX) == -1){
		exit(1);
	}

	if (semctl(semid, 1, SETVAL, (int)0) == -1){
		exit(1);
	}

	shmid = shmget(45281, MAX*sizeof(int), IPC_CREAT|0600);

	if (shmid == -1){
		exit(1);
}
	buf = (int*)shmat(shmid, NULL, 0);

	if (buf == NULL){
		exit(1);
}

	for (i=0; i<10000; i++){
  	opusc(semid, 0);
    printf("Zapisuje %d\n", i);
		buf[i%MAX] = i;
    sleep(1);
		podnies(semid, 1);
    sleep(4);
	}
}
