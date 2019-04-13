#include <stdio.h>
#include <stdlib.h>

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
              printf("I am %d child, with pid = %d, my parent: %d\n", i+1, getpid(), getppid());
              break;
            }
        }
    }
    return 0;
}
