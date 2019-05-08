  #define _GNU_SOURCE
  #include <sys/types.h>
  #include <sys/ipc.h>
  #include <sys/shm.h>
  #include <stdlib.h>
  #define MAX 10

  int main(){
    int shmid, i;
    int *buf;
    shmid = shmget(45281, MAX*sizeof(int), IPC_CREAT|0600);
    if (shmid == -1){
        exit(1);
    }

    buf = (int*)shmat(shmid, NULL, 0);
    if (buf == NULL){
        exit(2);
    }

    for (i=0; i<10000; i++)
        buf[i%MAX] = 0;
  }
