#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <ctype.h>
#include <math.h>
#include <semaphore.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>

#define QUEUE_KEY "/que1"
#define CHECK_IN_KEY "/c1he"
#define BOARDING_KEY "/bo1a"
#define RIDING_KEY "/rid1"
#define UNLOADING_KEY "/u1n"
#define CAR_QUEUE_KEY "/ca1r"


struct car_arg{
  int capacity;
  int no_of_rides;
  int id;
  struct timeval start_time;
};

struct passenger_arg{
  int id;
  int capacity;
  struct timeval start_time;
};


void initialize_semaphores();
void print_time(struct timeval tm1, struct timeval tm2);
void exit_function();
void sigint_signal(int signum);
