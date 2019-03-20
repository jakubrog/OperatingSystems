#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>


int generateFile(char *filePath, int bytes, int seconds) {
    FILE *file = fopen(filePath, "r+");
    int index = 0;

    int process_id = getpid();
    fprintf(file, "PID: %d SECONDS: %d  ", process_id, seconds);


    if (bytes < 1) {
        return 1;
    }

    if (!file) {
        printf("Cannot open  %s.\n", filePath);
        return 1;
    }

    FILE *random = fopen("/dev/urandom", "r"); // random nb generator

    if (!random) {
        printf("Cannot open /dev/urandom.\n");
        return 1;
    }

    char *tmp = malloc(bytes * sizeof(char));


    if (fread(tmp, sizeof(char), (size_t) bytes, random) != bytes) {  // copying random into tmp
        printf("Error while reading /dev/urandom.\n");
        return 1;
    }

    for (int j = index; j < bytes; j++)
        tmp[j] = (char) (abs(tmp[j])% 25 + 65);  // converting generated values into alphabet

    tmp[bytes - 1] = 10; // line feed (starting new line in a file after copying into it), it's also last byte in block

    if (fwrite(tmp,sizeof(char), (size_t) bytes, file) != bytes) {
        printf("Error while writing to %s.\n", filePath);
        return 1;
    }

    fclose(file);
    fclose(random);
    free(tmp);
    return 0;
};

int main(int argc, char **argv) {

    generateFile("/home/jakubrog/test", 10, 10);
    return 0;
}