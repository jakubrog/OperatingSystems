#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <memory.h>

void handler(int, siginfo_t *, void *);


int signals_to_send;
int signal_type;
int received = 0;
pid_t pid;
int sent = 0;


void printStats() {
    printf("Signals sent: %d\n", sent);
    printf("Signals received from catcher: %d\n", received);
}


int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Not enough arguments\n");
        exit(1);
    }

    pid = (int) strtol(argv[1], NULL, 10);

    signals_to_send = (int) strtol(argv[2], NULL, 10);

    signal_type = (int) strtol(argv[3], NULL, 10);

    if (signals_to_send < 1) {
        printf("Signals to send must be grater than 0\n");
        exit(1);
    }

    if (signal_type < 1 || signal_type > 3) {
        printf("Signal type is 1 - KILL, 2 - SIGQUEUE or 3 - SIGRT \n");
        exit(1);
    }


    for (; sent < signals_to_send; sent++) {

        switch (signal_type) {
            case 1 : {
                printf("Sending SIGUSR1\n");
                kill(pid, SIGUSR1);
                break;
            }
            case 2 : {
                union sigval value;
                value.sival_int = 0;
                value.sival_ptr = 0;
                printf("Sending SIGUSR1\n");
                sigqueue(pid, SIGUSR1, value);
                break;
            }
            default: {
                printf("Sending SIGRTMIN\n");
                kill(pid, SIGRTMIN);
            }
        }
    }

    kill(SIGUSR2, pid);
    printf("Sent SIGUSR2\n");


    struct sigaction act;
    act.sa_sigaction = handler;
    act.sa_flags = 0;

    while(1) {
        sigaction(SIGINT, &act, NULL);
        sigaction(SIGUSR1, &act, NULL);
        sigaction(SIGRTMIN, &act, NULL);
        sigaction(SIGUSR2, &act, NULL);
    }
}


void handler(int sig_num, siginfo_t *info, void *context) {
    if (sig_num == SIGUSR2 || sig_num == SIGINT) {
        kill(pid, SIGINT);
        printStats();
        exit(9);
    }
    if (info->si_pid != pid) return; // check if signal was from  catcher

    if (signal_type == 1 && sig_num == SIGUSR1) {
        received++;
    } else if (signal_type == 2 && sig_num == SIGUSR1) {
        received++;
    } else if (signal_type == 3 && sig_num == SIGRTMIN)
        received++;
}
