#define _GNU_SOURCE
#include "load.h"
//#include "sysopy.h"

void init();
void exit_function();
// double getCurrentTime();

int numberOfCycles = -1;
int packageLoad = 0;

int semaphoreId = -2;
int shm_id = -2;
struct belt_queue *belt;
sem_t *elem, *load, *queue;

int main(int argc, char **argv) {
  if (argc < 2)
    printf("Not enough arguments\n" );

  packageLoad = atoi(argv[1]);

  if (argc > 2)
    numberOfCycles = atoi(argv[2]);

  init();

  while (numberOfCycles > 0 || numberOfCycles == -1) {
    struct load newPackage;
    newPackage.loaderId = getpid();
    newPackage.weight = packageLoad;
    newPackage.time = getCurrentTime();

    // trying to get a semaphore to belt
    // takeBeltSem(semaphoreId, packageLoad);
    sem_wait(load);
    sem_wait(elem);
    sem_wait(queue);

    if(push(belt, newPackage) != -1)
        printf("[%f] Placed load on belt. Weight: %i\tPid: %i\n\t Belt current weight: %d\n", getCurrentTime(),
              packageLoad, getpid(), belt->currentWeight);


    sem_post(load);
    sem_post(elem);

    if (numberOfCycles != -1)
      numberOfCycles--;
  }
  printf("Finished\n");
  return 0;
}

void init() {

  atexit(exit_function);

  shm_id = shm_open(SHM_PATH,  O_RDWR, 0666);

  if(shm_id == -1){
    printf("Shared memeory not created\n" );
    exit(1);
  }
  printf("%d\n", shm_id );
  // atach shared memory to belt
  ftruncate(shm_id, sizeof(struct belt_queue));
  belt = mmap(NULL, sizeof(struct belt_queue),
                      PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);

  elem = sem_open(ELEM_SEM_NO,  O_RDWR);


  load = sem_open(LOAD_SEM_NO, O_RDWR );


  queue = sem_open(QUEUE_SEM_NO, O_RDWR );
}

void exit_function() {
  munmap(belt, sizeof(struct belt_queue));
  sem_close(elem);
  sem_close(queue);
  sem_close(load);
}
