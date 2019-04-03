#define _GNU_SOURCE
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int wait = 0;
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

    time_t t = time(NULL);

    while(1){
        sigaction(SIGTSTP, &action, NULL);
        signal(SIGINT, int_signal);

        if (!wait) {
            struct tm tm = *localtime(&t);
            printf("%d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        }
        sleep(1);
    }
}
