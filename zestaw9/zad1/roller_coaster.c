#include "roller_coaster.h"

static int on_board_passenger_counter = 0;
static int left_on_board_passengers = 0;


sem_t *queue, *check_in ,*boarding, *riding,*unloading, *car_queue ;


void *take_ride(void *args)
{
    struct passenger_arg *arg = (struct passenger_arg*) args;
    int passenger_id = arg->id;
    int capacity = arg->capacity;
    struct timeval tm1 = arg->start_time;
    struct timeval tm2;

    while(1){
        sem_wait(queue); // join the queue for a ride
        sem_wait(check_in); // time to check in


         gettimeofday(&tm2, NULL);
         print_time(tm1, tm2);
         printf("Passenger %d: on board with %d other passengers\n", passenger_id, on_board_passenger_counter);
         on_board_passenger_counter++;

         if (on_board_passenger_counter == capacity){
              gettimeofday(&tm2, NULL);
              print_time(tm1, tm2);
              printf("Passenger %d: clicked start\n", passenger_id);
              sem_post(boarding); // boaring completed
         }
        sem_post(check_in); // somone else can board now
        sem_wait(riding); // wait till end of ride

        gettimeofday(&tm2, NULL);
        print_time(tm1, tm2);
        left_on_board_passengers--;
        printf("Passenger %d: leaving board with %d other passengers\n", passenger_id, left_on_board_passengers);


        sem_post(unloading); // this passenger has already left the car
    }
    pthread_exit(0);
}

void * car_thread_func(void *args){
     int i, j;
     struct car_arg *arg = (struct car_arg*) args;
     int no_of_rides = arg->no_of_rides;
     int capacity = arg->capacity;
     int car_id = arg->id;
     struct timeval tm1 = arg->start_time;
     struct timeval tm2;
     sem_wait(car_queue);  // wait for your first run
     for (i = 1; i <= no_of_rides; i++) {
          gettimeofday(&tm2, NULL);
          print_time(tm1, tm2);
          printf("Car %d: Boarding  has started\n", car_id);

          for (j = 1; j <= capacity; j++){
               sem_post(queue); // allow capacity passengers to check in
           }
          sem_wait(boarding); // wait for boarding to complete
          on_board_passenger_counter = 0;
          gettimeofday(&tm2, NULL);
          print_time(tm1, tm2);
          printf("Car %d: Boarding completed, door closed\n", car_id);

          gettimeofday(&tm2, NULL);
          print_time(tm1, tm2);
          printf("Car %d: Ride started\n", car_id);

          sem_post(car_queue);

          usleep(100); // ride for a while

          sem_wait(car_queue); // wait to unload

          gettimeofday(&tm2, NULL);
          print_time(tm1, tm2);
          printf("Car %d: Ride ended, door open\n", car_id);
          //sem_wait(car_queue);

          left_on_board_passengers = capacity;

          for (j = 1; j <= capacity; j++) { // start unloading passengers
               sem_post(riding);       // release a passenger
               sem_wait(unloading);      // wait until this passenger is done
          }


          gettimeofday(&tm2, NULL);
          print_time(tm1, tm2);
          printf("Car %d: Unloading completed\n", car_id);

     }

     sem_post(car_queue);
     gettimeofday(&tm2, NULL);
     print_time(tm1, tm2);
     printf("Car %d: Ended for today\n", car_id );
     pthread_exit(0);
}


int main(int argc, char **argv){
    int no_of_passengers,  // number of passengers in the park
         capacity,            // capacity of the car
         no_of_rides, // number of times the car rides
         no_of_cars;  // number of cars in the park
    signal(SIGINT, sigint_signal);
    atexit(exit_function);
     if (argc != 5) {
          printf("Wrong no. of arguments\n");
          exit(1);
     }  else {
          no_of_passengers = atoi(argv[1]);
          no_of_cars = atoi(argv[2]);
          capacity = atoi(argv[3]);
          no_of_rides   = atoi(argv[4]);
     }

     if (capacity * no_of_cars > no_of_passengers) {
          printf("Number of passengers must be grater than (number of cars) * (capacity)\n");
          exit(1);
     }
     initialize_semaphores();
     // threads
     pthread_t cars[no_of_cars];
     pthread_t passengers[no_of_passengers];

     // arguments to threads
     struct car_arg car_args[no_of_cars];
     struct passenger_arg passenger_args[no_of_passengers];

     //current time as argument to thread
     struct timeval tm1;
     gettimeofday(&tm1, NULL);

     for(int i = 0; i < no_of_cars; i++){
         car_args[i].capacity = capacity;
         car_args[i].no_of_rides = no_of_rides;
         car_args[i].id = i+1;
         car_args[i].start_time = tm1;
         pthread_create(&cars[i], NULL, car_thread_func, &car_args[i]);
     }

     for(int i = 0; i < no_of_passengers; i++){
         passenger_args[i].capacity = capacity;
         passenger_args[i].id = i+1;
         passenger_args[i].start_time = tm1;
         pthread_create(&passengers[i], NULL, take_ride, &passenger_args[i]);
     }

     for(int i = 0; i<no_of_cars;i++)
        pthread_join(cars[i], NULL);

    struct timeval tm2;
    for(int i = 0; i < no_of_passengers; i++){
        gettimeofday(&tm2, NULL);
        print_time(tm1, tm2);
        printf("Passenger %d: dead\n", i+1);
        pthread_cancel(passengers[i]);
    }

    return 0;
}


void initialize_semaphores(){
    queue = sem_open(QUEUE_KEY, O_WRONLY | O_CREAT | O_EXCL,S_IRWXU | S_IRWXG, 0); // passenger waiting for a ride
    if(queue == SEM_FAILED){
        printf("Cannot create semaphore queue: %s \n", strerror(errno));
        exit(1);
    }
    check_in = sem_open(CHECK_IN_KEY,  O_WRONLY | O_CREAT | O_EXCL,S_IRWXU | S_IRWXG, 1);      // check in counter
    if(check_in == SEM_FAILED){
        printf("Cannot create semaphore check in: %s \n", strerror(errno));
        exit(1);
    }
    boarding = sem_open(BOARDING_KEY,  O_WRONLY | O_CREAT | O_EXCL,S_IRWXU | S_IRWXG, 0);  // controls the boarding process
    if(boarding == SEM_FAILED){
        printf("Cannot create semaphore boarding: %s \n", strerror(errno));
        exit(1);
    }
    riding = sem_open(RIDING_KEY, O_WRONLY | O_CREAT | O_EXCL,S_IRWXU | S_IRWXG, 0);      // keep passengers on the car so
    if(riding  == SEM_FAILED){
        printf("Cannot create semaphore riding: %s  \n", strerror(errno));
        exit(1);
    }
    unloading = sem_open(UNLOADING_KEY, O_WRONLY | O_CREAT | O_EXCL,S_IRWXU | S_IRWXG, 0);
    if(unloading == SEM_FAILED){
        printf("Cannot create semaphore unloading: %s\n", strerror(errno));
        exit(1);
    }
    car_queue = sem_open(CAR_QUEUE_KEY,  O_WRONLY | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, 1);
    if(car_queue == SEM_FAILED){
        printf("Cannot create semaphore car queue: %s\n", strerror(errno));
        exit(1);
    }

}

void print_time(struct timeval tm1, struct timeval tm2){
    printf("%ld us. ", tm2.tv_usec - tm1.tv_usec);
}

void exit_function(){
     sem_close(queue);
     sem_close(car_queue);
     sem_close(check_in);
     sem_close(boarding);
     sem_close(riding);
     sem_close(unloading);

     if(sem_unlink(QUEUE_KEY) != 0)
          printf("Cannot unlink queue semaphore: %s\n", strerror(errno));
     if(sem_unlink(CAR_QUEUE_KEY) != 0)
        printf("Cannot unlink car semaphore: %s\n", strerror(errno));
     if(sem_unlink(CHECK_IN_KEY) != 0)
        printf("Cannot unlink check in semaphore: %s\n", strerror(errno));
     if(sem_unlink(BOARDING_KEY) != 0)
        printf("Cannot unlink boarding semaphore: %s\n", strerror(errno));
     if(sem_unlink(RIDING_KEY) != 0)
        printf("Cannot unlink riding semaphore: %s\n", strerror(errno));
     if(sem_unlink(UNLOADING_KEY) != 0)
        printf("Cannot unlink unloading semaphore: %s\n", strerror(errno));
}
void sigint_signal(int signum){
    exit_function();
    exit(1);
}
