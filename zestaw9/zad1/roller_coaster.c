#include "roller_coaster.h"

static int on_board_passenger_counter = 0;

pthread_mutex_t queue = PTHREAD_MUTEX_INITIALIZER; // passenger waiting for a ride
pthread_mutex_t checkIn = PTHREAD_MUTEX_INITIALIZER;      // check in counter
pthread_mutex_t boarding = PTHREAD_MUTEX_INITIALIZER;  // controls the boarding process
pthread_mutex_t riding = PTHREAD_MUTEX_INITIALIZER;      // keep passengers on the car so
pthread_mutex_t unloading = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t car_queue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t wchodze = PTHREAD_MUTEX_INITIALIZER;



void *take_ride(void *args)
{
    struct passenger_arg *arg = (struct passenger_arg*) args;
    int passenger_id = arg->id;
    int capacity = arg->capacity;

    while(1){
         pthread_mutex_lock(&queue); // join the queue for a ride
         pthread_mutex_lock(&checkIn);         // time to check in

         // save my name and increase the counter
         printf("Passenger no. %d is on board with %d other passengers\n", passenger_id, on_board_passenger_counter);
         on_board_passenger_counter++;

         // if I am the last one to be on board, boarding completes and the car is full
         if (on_board_passenger_counter == capacity){
              on_board_passenger_counter = 0;
              pthread_mutex_unlock(&boarding);
          }

         pthread_mutex_unlock(&checkIn);        // allow next passenger to check in
         pthread_mutex_lock(&riding);          // I am riding in the car
         pthread_mutex_unlock(&unloading);   // get off the car
    }
    pthread_exit(0);
}

void * car_thread_func(void *args){
     int i, j;
     struct car_arg *arg = (struct car_arg*) args;
     int no_of_rides = arg->no_of_rides;
     int capacity = arg->capacity;

     for (i = 1; i <= no_of_rides; i++) {
          pthread_mutex_lock(&car_queue);
          printf("Door open\n");
          for (j = 1; j <= capacity; j++){
               pthread_mutex_unlock(&queue); // allow CAPACITY passengers to check in
           }

          pthread_mutex_lock(&boarding);       // wait for boarding to complete
          printf("Boarding completed\n");
          printf("Ride started\n");
          usleep(100);                     // ride for a while
          printf("Ride ended\n");
          on_board_passenger_counter = 0;  // start unloading passengers
          for (j = 1; j <= capacity; j++) {
               pthread_mutex_unlock(&riding);       // release a passenger
               pthread_mutex_lock(&unloading);      // wait until this passenger is done
          }
          printf("Unloading completed\n");
          // definitely, the car is empty and goes for the next run
          pthread_mutex_unlock(&car_queue);
     }
     // done here and show messages
     printf("The car is shot-off for maintenance\n");
     pthread_exit(0);
}


int main(int argc, char **argv){
    int no_of_passengers,  // number of passengers in the park
         capacity,            // capacity of the car
         no_of_rides, // number of times the car rides
         no_of_cars;  // number of cars in the park

     if (argc != 5) {
          printf("Wrong no. of arguments\n");
          exit(1);
     }  else {
          no_of_passengers = atoi(argv[1]);
          no_of_cars = atoi(argv[2]);
          capacity = atoi(argv[3]);
          no_of_rides   = atoi(argv[4]);
     }

     if (capacity > no_of_passengers) {
          printf("Car capacity must be smaller than number of passengers\n");
          exit(1);
     }

     // threads
     pthread_t cars[no_of_cars];
     pthread_t passengers[no_of_passengers];

     // arguments to threads
     struct car_arg car_args[no_of_cars];
     struct passenger_arg passenger_args[no_of_cars];
     pthread_mutex_lock(&queue);
     //pthread_mutex_lock(&checkIn);
     pthread_mutex_lock(&boarding);
     pthread_mutex_lock(&riding);
     pthread_mutex_lock(&unloading);
     //pthread_mutex_lock(&car_queue);


     for(int i = 0; i < no_of_cars; i++){
         car_args[i].capacity = capacity;
         car_args[i].no_of_rides = no_of_rides;
         pthread_create(&cars[i], NULL, car_thread_func, &car_args[i]);
     }

     for(int i = 0; i < no_of_cars; i++){
         passenger_args[i].capacity = capacity;
         passenger_args[i].id = i;
         pthread_create(&passengers[i], NULL, take_ride, &passenger_args[i]);
     }

     for(int i = 0; i<no_of_cars;i++)
        pthread_join(cars[i], NULL);


    pthread_mutex_destroy(&queue);
    pthread_mutex_destroy(&checkIn);
    pthread_mutex_destroy(&boarding);
    pthread_mutex_destroy(&riding);
    pthread_mutex_destroy(&unloading);
    pthread_mutex_destroy(&car_queue);
     return 0;
}
