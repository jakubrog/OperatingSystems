#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <memory.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>

int main(int argc, char **argv){


    if(argc != 3) {
        perror("Wrong arguments");
        exit(1);
    }

    int N = (int) strtol(argv[2], NULL, 10);


    FILE *pipe = fopen(argv[1], "w");

    if (pipe == NULL) {
        printf("Cannot open a pipe");
        exit(2);
    }

    pid_t pid = getpid();

    printf("%d\n", pid);

    srand(time(NULL));

    for(int i = 0; i < N; i++){

        FILE *dateOutput = popen("date", "r");  // pipe stream from date process

        char dateBuffer[32];
        fgets(dateBuffer, sizeof(dateBuffer), dateOutput); // read line from date process
        fclose(dateOutput);

        char pipeBuffer[PIPE_BUF];

        sprintf(pipeBuffer, "%d. PID: %d, date: %s\n", i + 1, pid, dateBuffer);   //  Instead of printing on console, it store output on char buffer

        fputs(pipeBuffer, pipe);  // write buffer to pipe

        fflush(pipe);  //flushes the output buffer of a stream

        sleep(rand() % 4 + 2);
    }
    fclose(pipe);
}
