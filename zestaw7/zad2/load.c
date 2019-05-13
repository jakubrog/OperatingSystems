#include "load.h"

double getCurrentTime() {
  struct timeval currentTime;
   gettimeofday(&currentTime, NULL);
   return currentTime.tv_sec + currentTime.tv_usec * 1.0 / 1e6;
}


int index_count(struct belt_queue *queue, int *index) {
  int oldIndex = *index;
  if (oldIndex + 1 >= queue->maxSize) {
    *index = 0;
  } else
    (*index)++;
  return oldIndex;
}

int push(struct belt_queue *queue, struct load elem) {
  if (queue == NULL || queue->maxSize == 0)
    printf("null queue");
  if (isFull(queue))
    printf("full queue");

  if(queue->currentWeight + elem.weight > queue->maxWeight )
      return -1;
  int index = index_count(queue, &queue->tail);
  queue->array[index] = elem;

  queue->currentWeight += elem.weight;
  queue->size++;
  return 0;
}

struct load pop(struct belt_queue *queue) {
  if (queue == NULL || queue->size == 0)
    printf("null queue");
  if (is_empty(queue))
    printf("full queue");

  int index = index_count(queue, &queue->head);
  queue->size--;

  queue->currentWeight -= queue->array[index].weight;
  return queue->array[index];
}

int is_empty(struct belt_queue *queue) {
   return queue->size == 0;
}

int isFull(struct belt_queue *queue) {
  return queue->size == queue->maxSize;
}
