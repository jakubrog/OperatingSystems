#include "load.h"
//#include "sysopy.h"

void init();
void exit_function();
// double getCurrentTime();

int numberOfCycles = -1;
int packageLoad = 0;

int semaphoreId = -2;
int sharedMemoryId = -2;
struct belt_queue *belt;

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
    takeBeltSem(semaphoreId, packageLoad);


    takeTruckSem(semaphoreId);

    if(push(belt, newPackage) != -1)
        printf("[%f] Placed load on belt. Weight: %i\tPid: %i\n\t Belt current weight: %d\n", getCurrentTime(),
              packageLoad, getpid(), belt->currentWeight);

    releaseTruckSem(semaphoreId);

    if (numberOfCycles != -1)
      numberOfCycles--;
  }
  printf("Finished\n");
  return 0;
}

void init() {

  atexit(exit_function);

  key_t key = TEMP_KEY;

  sharedMemoryId = shmget(key, sizeof(struct belt_queue) + 10, 0);

  belt = shmat(sharedMemoryId, 0, 0);

  semaphoreId = semget(key, 0, 0);
}

void exit_function() {
  shmdt(belt);
}
