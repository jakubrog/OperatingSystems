
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
    act.sa_flags = SA_SIGINFO;
    printf("%d\n", getpid());

    while(end == 0) {
        sigaction(SIGUSR1, &act, NULL);
        sigaction(SIGRTMIN, &act, NULL);
        sigaction(SIGUSR2, &act, NULL);
        sigaction(SIGINT, &act, NULL);
    }
    return 0;
}
void handler(int sig_num, siginfo_t *info, void *context){
    if(sig_num == SIGUSR1 || sig_num == SIGRTMIN){
        kill(pid, SIGUSR2);
        sleep(1);
        received ++;
        return;
    }else if(sig_num == SIGUSR2 || sig_num == SIGINT){
        pid = info->si_pid;
        for(; sent < received; sent++)
            kill(pid, SIGUSR1);
        kill(pid, SIGUSR2);
        end = 1;
        printStats();
        return;
    }
    printf("Unexpected signal\n");
}
