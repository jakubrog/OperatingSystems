#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <memory.h>
#include <fcntl.h>
#include <limits.h>


int main(int argc, char **argv) {
    if(argc != 2) {
        perror("Not enough arguments");
        exit(0);
    }

    if(mkfifo(argv[1], 0666) != 0){  //
        printf("Cannot create fifo");
        exit(1);
    }

    FILE *pipe = fopen(argv[1], "r");

    if (pipe == NULL) {
        printf("Cannot open a fifo.");
        exit(2);
    }

    char pipeBuffer[PIPE_BUF];  // PIPE_BUF = 4096

    while(fgets(pipeBuffer, PIPE_BUF, pipe)){
        printf("%s", pipeBuffer);
    }

    fclose(pipe);
}
