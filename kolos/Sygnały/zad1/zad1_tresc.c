#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// zablokuj wszystkie sygnaly w potomku po za SIGUSR, niech program macierzysty
// niech wejscie definiuje to co program macierzysyty ma wyslac do dziecka a
// dziecko ma to wyswietlic w handlerze lub zablokowac

int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;


    int child = fork();

    if(child == 0){

    }else{

    }

  }
