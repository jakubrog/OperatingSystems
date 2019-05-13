#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#ifndef LOAD_H
#define LOAD_H

#define MAX_LOAD 100
#define MAX_QUEUE_SIZE 100
#define TEMP_KEY 101

#define ELEM_SEM_NO 0
#define LOAD_SEM_NO 1
#define QUEUE_SEM_NO 2

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
  struct load array[MAX_QUEUE_SIZE];
};

void init_belt_queue(struct belt_queue *queue);
void push(struct belt_queue *queue, struct load elem);
struct load pop(struct belt_queue *queue);
int is_empty(struct belt_queue *queue);
int isFull(struct belt_queue *queue);
void takeTruckSem(int semid);
void releaseTruckSem(int semid);
void takeBeltSem(int semid, int weight);
void releaseBeltSem(int semid, int weight);
double getCurrentTime();
#endif
