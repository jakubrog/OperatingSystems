#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <memory.h>

void handler(int sig_num, siginfo_t *info, void *context);


int sent = 0;
int received = 0;
pid_t pid = 0;
int end = 0;


void printStats() {
    printf("Signals sent: %d\n", sent);
    printf("Signals received from catcher: %d\n", received);
}


int main(int argc, char *argv[]) {

    struct sigaction act;
    act.sa_sigaction = handler;
    act.sa_flags = 0;
    printf("%d\n", getpid());

    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGRTMIN, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
    sigaction(SIGINT, &act, NULL);
    while(1) {
        pause();
    }
    return 0;
}
void handler(int sig_num, siginfo_t *info, void *context){
    if(sig_num == SIGUSR1 || sig_num == SIGRTMIN){
        received ++;
        return;
    }else if(sig_num == SIGUSR2 || sig_num == SIGINT){
        for(; sent < received; sent++)
            kill(info->si_pid, SIGUSR1);
        kill(info->si_pid, SIGUSR2);
        end = 1;
        printStats();
        exit(0);
    }
    printf("Unexpected signal\n");
}
