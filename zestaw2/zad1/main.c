#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/times.h>




int generateFile(char *filePath, int numberOfRecords, int recordSize) {
    FILE *file = fopen(filePath, "w+"); // w+ = write + read

    if(numberOfRecords < 1 || recordSize < 1){
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

    char *tmp = malloc(recordSize * sizeof(char)); // copy of record

    for (int i = 0; i < numberOfRecords; ++i) {
        if (fread(tmp, sizeof(char), (size_t) recordSize, random) != recordSize) {  // copying random into tmp
            printf("Error while reading /dev/urandom.\n");
            return 1;
        }

        for (int j = 0; j < recordSize; j++)
            tmp[j] = (char) (abs(tmp[j]) % 25 + 65);  // converting generated values into alphabet

        tmp[recordSize-1] = 10; // line feed (starting new line in a file after copying into it), it's also last byte in block

        if (fwrite(tmp, sizeof(char), (size_t) recordSize, file) != recordSize) {
            printf("Error while writing to %s.\n", filePath);
            return 1;
        }
    }
    fclose(file);
    fclose(random);
    free(tmp);
    return 0;
};


int lib_sort(char *path, int numberOfRecords, int recordSize) {
    FILE *file = fopen(path, "r+");
    if(!file)
        return -1;

    char *tmp1 = malloc((recordSize) * sizeof(char)); // to keep blocks when they are compared and swapped
    char *tmp2 = malloc((recordSize) * sizeof(char));

    long int offset = (long int) (recordSize) * sizeof(char);  // bytes in one row


    for (int i = 0; i < numberOfRecords; i++) {
        fseek(file, i * offset, 0);

        if( fread(tmp1, sizeof(char), (size_t)(recordSize), file) != recordSize )
            return 1;

        for (int j = 0; j < i; j++) {
            fseek(file, j * offset, 0);

            if (fread(tmp2, sizeof(char), (size_t)(recordSize), file) != recordSize ) {
                return 2;
            }

            if (tmp2[0] > tmp1[0]) {               // swaping records in file
                fseek(file, j * offset, 0);
                if (fwrite(tmp1, sizeof(char), (size_t)(recordSize), file) != recordSize) {
                    return 3;
                }
                fseek(file, i * offset, 0);
                if (fwrite(tmp2, sizeof(char), (size_t)(recordSize), file) != recordSize) {
                    return 4;
                }
                char *tmp = tmp1;
                tmp1 = tmp2;
                tmp2 = tmp;
            }
        }
    }

    fclose(file);
    free(tmp1);
    free(tmp2);
    return 0;
};

int sys_sort(char *path, int numberOfRecords, int recordSize) {
    int file = open(path, O_RDWR); // O_RDWR - open for reading and writing

    if(file < 0) // -1 means error
        return 1;

    char *tmp1 = malloc(recordSize * sizeof(char));
    char *tmp2 = malloc(recordSize * sizeof(char));

    long int offset = (long int) (recordSize * sizeof(char)); // how many bytes to move to next row

    for (int i = 0; i < numberOfRecords; i++) {

        lseek(file, i * offset, SEEK_SET); // SEEK_SET - from begining

        if (read(file, tmp1, (size_t) offset) != recordSize) { //read(from, where, how many)
            return 1;
        }

        for (int j = 0; j < i; j++) {
            lseek(file, j * offset, SEEK_SET);
            if (read(file, tmp2, (size_t) offset) != recordSize) {
                return 1;
            }
            if (tmp2[0] > tmp1[0]) {

                lseek(file, j * offset, 0);

                if (write(file, tmp1, (size_t) offset) != recordSize) {
                    return 1;
                }

                lseek(file, i * offset, 0);

                if (write(file, tmp2, (size_t) offset) != recordSize) {
                    return 1;
                }
                char *tmp = tmp1;
                tmp1 = tmp2;
                tmp2 = tmp;
            }
        }
    }

    close(file);
    free(tmp1);
    free(tmp2);
    return 0;
}


int lib_copy(char *path, char *dest, int recordSize, int numberOfRecords) {
    FILE *src = fopen(path, "r");
    FILE *destination = fopen(dest, "w+");

    if (!src)
        return 1;
    if (!destination)
        return 4;

    char *tmp = malloc(sizeof(char) * recordSize);

    for (int i = 0; i < numberOfRecords; i++) {

        if (fread(tmp, sizeof(char), (size_t) (recordSize), src) != recordSize)  // reading from file to one line buffer
            return 2;

        if (fwrite(tmp, sizeof(char), (size_t) (recordSize), destination) != recordSize) // writing one line into buffer
            return 3;
    }
    fclose(src);
    fclose(destination);
    return 0;
}


int sys_copy(char *sourcePath, char *targetPath, int recordSize, int numberOfRecords){
    int source = open(sourcePath, O_RDONLY);

    if(source < 0){
        printf("Error while opening %s.\n", sourcePath);
        return 1;
    }

    int target = open(targetPath, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);

    if(target < 0){
        printf("Error while opening %s.\n", targetPath);
        return 1;
    }

    char *tmp = malloc(recordSize * sizeof(char));  //to keep one line of generated text

    for (int i = 0; i < numberOfRecords; i++){
        if(read(source, tmp, (size_t) recordSize * sizeof(char)) != recordSize)
            return 1;

        if(write(target, tmp, (size_t) recordSize * sizeof(char)) != recordSize)
            return 1;
    }

    close(source);
    close(target);
    free(tmp);
    return 0;
};

double timeDiff(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

void printTime(clock_t rStartTime, struct tms tmsStartTime, clock_t rEndTime, struct tms tmsEndTime){
    printf("Real:   %.2lf s   ", timeDiff(rStartTime, rEndTime));
    printf("System: %.2lf s\n", timeDiff(tmsStartTime.tms_stime, tmsEndTime.tms_stime));
}





int main(int argc, char *argv[]) {
    if(argc < 5){
        printf("Not enough arguments");
        return 1;
    }
    clock_t rTime[3] = {0, 0, 0};
    struct tms* tmsTime[6];

    for (int i = 0; i < 6; i++) {
        tmsTime[i] = malloc(sizeof(struct tms*));
    }

    if(strcmp(argv[1], "generate" ) == 0){

        int recordSize = (int)strtol(argv[4], NULL, 10);
        int numberOfRecords = (int)strtol(argv[3], NULL, 10);
        char *path = argv[2];

        if(generateFile(path, numberOfRecords, recordSize) != 0) {
            printf("Something went wrong");
            return 1;
        }

    }else if(strcmp(argv[1], "sort" ) == 0){

        if(argc < 6){
            printf("Not enough arguments");
            return 1;
        }

        int recordSize = (int)strtol(argv[4], NULL, 10); // converting char to int
        int numberOfRecords = (int)strtol(argv[3], NULL, 10);
        char *path = argv[2];




        if (strcmp(argv[5], "sys") == 0) {
            int currentTime = 0;

            rTime[currentTime] = times(tmsTime[currentTime]);
            currentTime++;

            sys_sort(path, numberOfRecords, recordSize);

            rTime[currentTime] = times(tmsTime[currentTime]);
            printTime(rTime[0], *tmsTime[0], rTime[1], *tmsTime[1]);

        }else if(strcmp(argv[5], "lib") == 0) {
            int currentTime = 0;
            rTime[currentTime] = times(tmsTime[currentTime]);
            currentTime++;


            lib_sort(path, numberOfRecords, recordSize);

            rTime[currentTime] = times(tmsTime[currentTime]);
            currentTime++;
            printTime(rTime[0], *tmsTime[0], rTime[1], *tmsTime[1]);

        }else{
            printf("Wrong sixth argument");
        };

    }else if(strcmp(argv[1], "copy" ) == 0){

        if(argc < 7){
            printf("Not enough arguments");
            return 1;
        }

        int recordSize = (int)strtol(argv[5], NULL, 10);
        int numberOfRecords = (int)strtol(argv[4], NULL, 10);
        char *source = argv[2];
        char *destination = argv[3];


        if (strcmp(argv[6], "sys") == 0) {

            int currentTime = 0;
            rTime[currentTime] = times(tmsTime[currentTime]);
            currentTime++;

            sys_copy(source, destination, recordSize, numberOfRecords);

            rTime[currentTime] = times(tmsTime[currentTime]);
            currentTime++;
            printTime(rTime[currentTime-2], *tmsTime[currentTime-2], rTime[currentTime-1], *tmsTime[currentTime-1]);

        }else if(strcmp(argv[6], "lib") == 0) {
            int currentTime = 0;
            rTime[currentTime] = times(tmsTime[currentTime]);
            currentTime++;

            lib_copy(source, destination, recordSize, numberOfRecords);

            rTime[currentTime] = times(tmsTime[currentTime]);
            currentTime++;
            printTime(rTime[currentTime-2], *tmsTime[currentTime-2], rTime[currentTime-1], *tmsTime[currentTime-1]);
        }else{
            printf("Wrong seventh argument");
        }
    }

    return 0;
}
