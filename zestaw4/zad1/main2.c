#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int wait = 0;
int alive_process = 1;

void stop_signal(int sig_num) {
    if(wait == 0) {
        printf("\nOdebrano sygnał %d \nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n", sig_num);
    }
    wait = wait == 0 ? 1 : 0 ;
}

void int_signal(int sig_num) {
    printf("\nOdebrano sygnał SIGINT\n");
    exit(0);
}

int main(int argc, char** argv) {

    struct sigaction action;

    action.sa_handler = stop_signal;
    action.sa_flags = 0;

    pid = fork();
    if (pid == 0){
        execl("./script.sh", "./script.sh", NULL);
        exit(0);
    }

    while(1){
      sigaction(SIGTSTP, &action, NULL);
      signal(SIGINT, int_signal);
      if(!wait){
        if(!alive_process){
          alive_process == 0;

          pid = fork();
          if(pid == 0){
            execl("./script.sh", "./script.sh", NULL);
            exit(0);
          }
        }
        }
      }
    }
}
