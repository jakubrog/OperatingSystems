#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// First argument - file to edit
// Second argument - min frequency
// Third argument - max frequency
// Fourth argument - nb of bytes to add

int generateFile(char *filePath, int bytes, int pmin, int pmax) {
    FILE *file = fopen(filePath, "r+"), *random;
    int index = 0;

    int process_id = getpid();


    if (bytes < 1) {
        return 1;
    }

    if (!file) {
        printf("Cannot open  %s.\n", filePath);
        return 1;
    }

    char *tmp = malloc(bytes * sizeof(char));
    int seconds = 1;
    time_t timer;
    char time_string[26];
    struct tm* tm_info;
    file = fopen(filePath, "a");
    srand(time(NULL));

    //main generating loop
    while(1){
        time(&timer);

        tm_info = localtime(&timer);

        strftime(time_string, 26, "%Y-%m-%d %H:%M:%S", tm_info);



        random = fopen("/dev/urandom", "r");

        if (!random) {
            printf("Cannot open /dev/urandom.\n");
            return 1;
        }
        seconds = (rand() % (pmax - pmin)) + pmin;


        if (fread(tmp, sizeof(char), (size_t) bytes, random) != bytes) {  // copying random into tmp
            printf("Error while reading /dev/urandom.\n");
            return 1;
        }


        for (int j = index; j < bytes; j++)
            tmp[j] = (char) (abs(tmp[j])% 25 + 65);  // converting generated values into alphabet

        tmp[bytes - 1] = 10; // line feed (starting new line in a file after copying into it), it's also last byte in block

        fprintf(file, "PID: %d SECONDS: %d  %s  %s\n", process_id, seconds, time_string, tmp);
        fflush(file);



        fclose(random);
        sleep(seconds);
    }
};

int main(int argc, char **argv) {
    if(argc < 4){
        printf("Not enough arguments.");
        return -1;
    }

    char *path = argv[1];
    int pmin = (int) strtol(argv[2], NULL, 10);
    int pmax = (int) strtol(argv[3], NULL, 10);
    int bytes = (int) strtol(argv[4], NULL, 10);

    generateFile(path, bytes, pmin, pmax);
    return 0;
}
