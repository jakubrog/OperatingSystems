#include "load.h"
//#include "sysopy.h"

void init();
void exit_function();
void signalHandler(int signo);
void new_truck();

int truckMaxLoad, queue_elements, max_weight;
int occupiedSpace;
int shm_id = -2;
int semaphoreId = -2;
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
      releaseBeltSem(semaphoreId, new_load.weight);
    } else {
      printf("Waiting for package\n");
      sleep(1);
    }
  }
  return 0;
}

void init() {
  // init queue
  key_t key = TEMP_KEY;

  // CREATING SHARED MEMORY
  shm_id = shmget(TEMP_KEY, sizeof(struct belt_queue) + 10, IPC_CREAT | IPC_EXCL | 0666);

  if(shm_id == -1){
    printf("Shared memeory not created\n" );
    exit(1);
  }
  printf("%d\n", shm_id );
  // atach shared memory to belt
  belt = shmat(shm_id, 0, 0);

  belt->head = 0;
  belt->tail = 0;
  belt->maxSize = MAX_QUEUE_SIZE;
  belt->size = 0;
  belt->maxWeight = max_weight;
  belt->currentWeight = 0;
  /// CREATING SEMAPHORES

  semaphoreId = semget(key, 3, IPC_CREAT | IPC_EXCL | 0666);

  // set semaphore to 0
  semctl(semaphoreId, ELEM_SEM_NO, SETVAL, queue_elements);

  // set semaphore to 1
  semctl(semaphoreId, LOAD_SEM_NO, SETVAL, max_weight);

  // set semaphore to 2
  semctl(semaphoreId, QUEUE_SEM_NO, SETVAL, 1);


  new_truck();
}


void exit_function() {
  // detach shared memory
  shmdt(belt);

  if (shm_id >= 0) {
    shmctl(shm_id, IPC_RMID, NULL);
  }
  if (semaphoreId >= 0) {
    semctl(semaphoreId, 0, IPC_RMID);
  }
}

void signalHandler(int signo) {
    exit(0);
}

void new_truck() {
  printf("\n\nNew truck\n\n");
  belt->currentWeight = 0;
  occupiedSpace = 0;
}
