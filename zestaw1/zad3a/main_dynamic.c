#include <stdio.h>

#include <sys/times.h>
#include <unistd.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <memory.h>

double timeDiff(clock_t start, clock_t end){
    return (double)(end -  start) / sysconf(_SC_CLK_TCK); // whats that?
}

//TODO : and what is that
void printTime(clock_t rStartTime, struct tms tmsStartTime, clock_t rEndTime, struct tms tmsEndTime){
    printf("Real:   %.2lf s   ", timeDiff(rStartTime, rEndTime));
    printf("User:   %.2lf s   ", timeDiff(tmsStartTime.tms_utime, tmsEndTime.tms_utime));
    printf("System: %.2lf s\n", timeDiff(tmsStartTime.tms_stime, tmsEndTime.tms_stime));
}


void *dl_handle;   /// using dynamic library how does is work?
struct Array *(*createArray)(int);
void (*setDirectory)(struct Array*, char*);
void (*setFile)(struct Array*, char*);
void (*setResultFile)(struct Array*, char*);
void (*searchForFile)(struct Array*);
void (*deleteBlockAtIndex)(struct Array*, int);
void (*deleteArray)(struct Array*);
void (*print)();

typedef void *(*arbitrary)();

int main(int argc, char **argv){

    dl_handle = dlopen("./library.so", RTLD_LAZY);
    if (!dl_handle) {
        printf("!!! %s\n", dlerror());
        return 1;
    }
    print = dlsym(dl_handle, "printa");
    if(print)
      printf("hello");

    struct Array *blockArray = NULL;

    clock_t rTime[6] = {0, 0, 0, 0, 0, 0}; /// TODO: whats that
    struct tms* tmsTime[6];

    for (int i = 0; i < 6; i++) {
        tmsTime[i] = malloc(sizeof(struct tms*));
    }

    if(argc < 2) {
        printf("Wrong arguments");
        return 1;
    }

    for(int index = 0 ; index < argc - 1; index++) {
        int currentTime = 0;

        if (strcmp(argv[index], "create_table") == 0) {

            index++;
            int number = 0;

            number = (int) strtol(argv[index], NULL, 10);

            rTime[currentTime] = times(tmsTime[currentTime]);
            currentTime++;

            createArray = dlsym(dl_handle,"createArray");
            if(createArray){
              printf("hera");
              blockArray = createArray(number);
            }else{
              printf("cannot find");
            }
            //arbitrary fcreate;
            //*(void **) (&fcreate) = dlsym(dl_handle, "createArray");


            rTime[currentTime] = times(tmsTime[currentTime]);
            currentTime++;

            printf("Creating array:\n");  /// TODO: why it looks like that
            printTime(rTime[currentTime-2], *tmsTime[currentTime-2], rTime[currentTime-1], *tmsTime[currentTime-1]);

        } else if (strcmp(argv[index], "search_directory") == 0) {

            if (index + 3 >= argc) {
                printf("Wrong arguments");
                return 1;
            }
            if (blockArray != NULL) {
                index++;
                setDirectory = dlsym(dl_handle,"setDirectory");
                setDirectory(blockArray, argv[index]);
                index++;
                setFile = dlsym(dl_handle,"setFile");
                setFile(blockArray, argv[index]);
                index++;
                setResultFile = dlsym(dl_handle,"setResultFile");
                setResultFile(blockArray, argv[index]);
            } else {
                printf("First you need to use create_table command");
                return 1;
            }
            rTime[currentTime] = times(tmsTime[currentTime]);
            currentTime++;
            searchForFile = dlsym(dl_handle,"searchForFile");
            searchForFile(blockArray);

            rTime[currentTime] = times(tmsTime[currentTime]);
            currentTime++;

            printf("Searching and saving into blocks:\n");
            printTime(rTime[currentTime-2], *tmsTime[currentTime-2], rTime[currentTime-1], *tmsTime[currentTime-1]);


        } else if (strcmp(argv[index], "remove_block") == 0) {
            int number = 0;
            number = (int) strtol(argv[index], NULL, 10);

            rTime[currentTime] = times(tmsTime[currentTime]);
            currentTime++;

            searchForFile = dlsym(dl_handle,"deleteBlockAtIndex");
            deleteBlockAtIndex(blockArray, number);

            rTime[currentTime] = times(tmsTime[currentTime]);
            currentTime++;


            printf("Delete one block:\n");
            printTime(rTime[currentTime-2], *tmsTime[currentTime-2], rTime[currentTime-1], *tmsTime[currentTime-1]);
        }


    }
    deleteArray = dlsym(dl_handle,"deleteArray");
    deleteArray(blockArray);

    return 0;
}
