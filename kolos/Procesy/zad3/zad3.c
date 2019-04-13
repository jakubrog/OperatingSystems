//*********************************************************
//Uzupelnij petle w taki sposob aby stworzyc dokladnie argv[1] procesow potomnych, bedacych dziecmi
//   tego samego procesu macierzystego.
   // Kazdy proces potomny powinien:
       // - "powiedziec ktorym jest dzieckiem",
        //-  jaki ma pid,
        //- kto jest jego rodzicem
   //******************************************************

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ( int argc, char *argv[] )
{
  int i, pid;

  if(argc != 2){
  printf("Not a suitable number of program arguments");
  exit(2);
  }else {
       for (i = 0; i < atoi(argv[1]); i++) {
            int child = fork();
            if(child == 0){
              printf("Hello i am %d child, I have pid equal to %d, my parent is %d.\n",i+1, getpid(), getppid());
              exit(0);
            }
        }
  }
}
