#include <fcntl.h>
#include <limits.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


#ifndef LOAD_H
#define LOAD_H

#define MAX_LOAD 100
#define MAX_QUEUE_SIZE 100
#define SHM_PATH "/homss"

#define ELEM_SEM_NO "/last"
#define LOAD_SEM_NO "/second"
#define QUEUE_SEM_NO "/third"


typedef pid_t loaderId;

struct load {
  int weight;
  loaderId loaderId;
  double time;
};

struct belt_queue {
  int head;
  int tail;
  int size;
  int maxSize;
  int currentWeight;
  int maxWeight;
  struct load array[MAX_QUEUE_SIZE];
};

void init_belt_queue(struct belt_queue *queue);
int push(struct belt_queue *queue, struct load elem);
struct load pop(struct belt_queue *queue);
int is_empty(struct belt_queue *queue);
int isFull(struct belt_queue *queue);
void takeTruckSem(int semid);
void releaseTruckSem(int semid);
void takeBeltSem(int semid, int weight);
void releaseBeltSem(int semid, int weight);
double getCurrentTime();
#endif
