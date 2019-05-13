#include "load.h"

double getCurrentTime() {
  struct timeval currentTime;
   gettimeofday(&currentTime, NULL);
   return currentTime.tv_sec + currentTime.tv_usec * 1.0 / 1e6;
}

// adding to queue semaphores
void takeTruckSem(int semid) {
  struct sembuf buf;
  buf.sem_num = QUEUE_SEM_NO;
  buf.sem_op = -1;
  buf.sem_flg = 0;
  if (semop(semid, &buf, 1) == -1)
    printf("Cannot take queue semaphore\n");
}

void releaseTruckSem(int semid) {
  struct sembuf buf;
  buf.sem_num = QUEUE_SEM_NO;
  buf.sem_op = 1;
  buf.sem_flg = 0;
  if (semop(semid, &buf, 1) == -1)
    printf("Cannot release queue semaphore\n" );
}
//////////////////////////////////////////////

//belt semaphores
void takeBeltSem(int semid, int weight) {
  struct sembuf buf;
  buf.sem_num = ELEM_SEM_NO;
  buf.sem_op = -1;
  buf.sem_flg = 0;
  if (semop(semid, &buf, 1) == -1)
    printf("Taking conveyor elem semaphore");

  buf.sem_num = LOAD_SEM_NO;
  buf.sem_op = -weight;

  if (semop(semid, &buf, 1) == -1)
    printf("Taking conveyor weight semaphore");
}

void releaseBeltSem(int semid, int weight) {
  struct sembuf buf;
  buf.sem_num = ELEM_SEM_NO;
  buf.sem_op = 1;
  buf.sem_flg = 0;

  if (semop(semid, &buf, 1) == -1)
    printf("Releasing conveyor elem semaphore");

  buf.sem_num = LOAD_SEM_NO;
  buf.sem_op = weight;
  if (semop(semid, &buf, 1) == -1)
    printf("Releasing conveyor weight semaphore");
}
////////////////////

//
int index_count(struct belt_queue *queue, int *index) {
  int oldIndex = *index;
  if (oldIndex + 1 >= queue->maxSize) {
    *index = 0;
  } else
    (*index)++;
  return oldIndex;
}

void push(struct belt_queue *queue, struct load elem) {
  if (queue == NULL || queue->maxSize == 0)
    printf("null queue");
  if (isFull(queue))
    printf("full queue");
  queue->array[index_count(queue, &queue->tail)] = elem;
  queue->size++;
}

struct load pop(struct belt_queue *queue) {
  if (queue == NULL || queue->size == 0)
    printf("null queue");
  if (is_empty(queue))
    printf("full queue");

  queue->size--;
  return queue->array[index_count(queue, &queue->head)];
}

int is_empty(struct belt_queue *queue) {
   return queue->size == 0;
}

int isFull(struct belt_queue *queue) {
  return queue->size == queue->maxSize;
}
