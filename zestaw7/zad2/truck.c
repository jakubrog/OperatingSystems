#include "load.h"
//#include "sysopy.h"

void init();
void exit_function();
void signalHandler(int signo);
void new_truck();

int truckMaxLoad, queue_elements, max_weight;
int occupiedSpace;
int shm_id = -2;
sem_t *elem, *load, *queue;
struct belt_queue *belt;

int main(int argc, char **argv) {

  signal(SIGINT, signalHandler);

  if (argc < 4){
    printf("Not enough arguments");
    exit(1);
  }

  truckMaxLoad = atoi(argv[1]);
  queue_elements = atoi(argv[2]);
  max_weight = atoi(argv[3]);

  if (queue_elements > MAX_QUEUE_SIZE){
    printf("queue is to big\n");
    exit(2);
  }

  // initialize queue and semaphores
  init();

  while (1) {
    if(!is_empty(belt)) {

      struct load new_load = pop(belt); // comming load

      if (new_load.weight + occupiedSpace >= truckMaxLoad ) {
         new_truck();
      }
      occupiedSpace += new_load.weight;

      printf("loaderId: %i\toccupied space: %i\tpackage weight: %i\tdiff time: "
             "%f\n",
             new_load.loaderId, occupiedSpace, new_load.weight,
             (getCurrentTime() - new_load.time));

      // realising belt semaphore

      sem_post(queue);



    } else {
      printf("Waiting for package\n");
      sleep(1);
    }
  }
  return 0;
}

void init() {

  // CREATING SHARED MEMORY
  atexit(exit_function);
  shm_id = shm_open(SHM_PATH, O_CREAT | O_RDWR | O_EXCL, 0666);

  if(shm_id == -1){
    printf("Shared memeory not created\n" );
    exit(1);
  }


  // atach shared memory to belt
  ftruncate(shm_id, sizeof(struct belt_queue));
  belt = mmap(NULL, sizeof(struct belt_queue),
                      PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
  belt->head = 0;
  belt->tail = 0;
  belt->maxSize = MAX_QUEUE_SIZE;
  belt->size = 0;
  belt->maxWeight = max_weight;
  belt->currentWeight = 0;
  /// CREATING SEMAPHORES

  elem = sem_open(ELEM_SEM_NO, O_CREAT | O_RDWR | O_EXCL, 0666, queue_elements);
  if(elem == SEM_FAILED)
    printf("elem\n" );

  load = sem_open(LOAD_SEM_NO, O_CREAT | O_RDWR | O_EXCL, 0666, max_weight);
  if(load == SEM_FAILED)
    printf("load\n" );


  queue = sem_open(QUEUE_SEM_NO, O_CREAT | O_RDWR | O_EXCL, 0666, 1);
  if(queue == SEM_FAILED)
    printf("queue\n" );


  new_truck();
}


void exit_function() {
  // detach shared memory
  shm_unlink(SHM_PATH);
  sem_close(elem);
  sem_close(queue);
  sem_close(load);
  sem_unlink(QUEUE_SEM_NO);
  sem_unlink(LOAD_SEM_NO);
  sem_unlink(ELEM_SEM_NO);
}

void signalHandler(int signo) {
    exit(0);
}

void new_truck() {
  printf("\n\nNew truck\n\n");
  belt->currentWeight = 0;
  occupiedSpace = 0;
}
