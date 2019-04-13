#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sighandler(int pid, siginfo_t *info, void *com){
  printf("Got singal %d, with : %d\n", info->si_signo, info->si_value.sival_int);
}


int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sighandler;
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    action.sa_mask = mask;
    action.sa_flags = SA_SIGINFO;



    int child = fork();
    if(child == 0) {
      sigaction(SIGUSR1, &action, NULL);
      pause();
    }
    else {
      sleep(1);
      union sigval sig;
      sig.sival_int = atoi(argv[1]);
      sigqueue(child, atoi(argv[2]), sig);
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]
    }

    return 0;
}
