#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>


struct car_arg{
  int capacity;
  int no_of_rides;
};

struct passenger_arg{
  int id;
  int capacity;
};
