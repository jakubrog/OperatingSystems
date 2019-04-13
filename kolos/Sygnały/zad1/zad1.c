#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(int pid, siginfo_t *sig, void *context ){
    printf("Child: %d\n", sig->si_value.sival_int);
}

// zablokuj wszystkie sygnaly w potomku po za SIGUSR,

int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &handler;
    action.sa_flags = SA_SIGINFO;
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    // action.sa_mask = mask;

    int child = fork();

    if(child == 0){
      sigprocmask(SIG_BLOCK, &mask);
      sigaction(SIGUSR1, &action, NULL);
      pause();
    }else{
        sleep(1); // po to zeby zdazyl wyslac przed dzieckiem
        union sigval sig;
        sig.sival_int = atoi(argv[1]);
        sigqueue(child, SIGUSR1, sig);
    }

  }
