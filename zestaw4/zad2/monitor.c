#include <signal.h>
#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>


// First argument is path to file which contains paths with time
// Second argument is time of watching
// third argumnt: copy - use copy type of monitor, else it will use second type


struct Files {
    char **path;   // array of paths to every file
    int *freq_time; // monitoring time of every file
    int files_to_monitor;  // number of files to monitor
};

int process_stopped = 0;
int killed = 0;


/// elapsed_time - difference between start time and current time
double elapsed_time(time_t start) {
    return difftime(time(NULL), start);
}


/// get_directory_path - extracts directory path from full path
char *get_directory_path(char *fullpath) {
    char *e = strrchr(fullpath, '/');
    if (!e) {
        return fullpath;
    }
    int index = (int) (e - fullpath);
    char *s = (char *) malloc(sizeof(char) * (index + 1));
    strncpy(s, fullpath, index);
    s[index] = '\0';
    return s;
}


/// get_file_name - creates full filename with path
char *get_file_name(time_t time1, char *path) {
    struct tm *time = localtime(&time1);
    char *result = calloc(strlen(path) + 20, sizeof(char));
    char *temp = calloc(30, sizeof(char));

    strcat(result, get_directory_path(path));
    strcat(result, "/archive/");
    strcat(result, basename(path));
    strcat(result, "_");

    sprintf(temp, "%d", time->tm_year + 1900);

    strcat(result, temp);
    strcat(result, "-");

    sprintf(temp, "%d", time->tm_mon + 1);

    strcat(result, temp);
    strcat(result, "-");

    sprintf(temp, "%d", time->tm_mday);

    strcat(result, temp);
    strcat(result, "_");

    sprintf(temp, "%d", time->tm_hour);

    strcat(result, temp);
    strcat(result, "-");

    sprintf(temp, "%d", time->tm_min);

    strcat(result, temp);
    strcat(result, "-");

    sprintf(temp, "%d", time->tm_sec);

    strcat(result, temp);

    return result;
}

struct Files *parse(char *path) {

    FILE *fp;
    struct Files *result = calloc(1, sizeof(struct Files));
    int files_to_monitor = 0;

    // Open the file
    fp = fopen(path, "r");

    // Check if file exists
    if (fp == NULL) {
        printf("Could not open file %s", path);
        exit(1);
    }

    // check how many files is going to be observed
    for (char c = (char) getc(fp); c != EOF; c = (char) getc(fp)) {
        if (c == '\n') // Increment count if this character is newline
            files_to_monitor++;
    }


    result->freq_time = calloc((size_t)files_to_monitor, sizeof(int));

    result->path = (char **) calloc(files_to_monitor , sizeof(char *));

    rewind(fp);

    int index;

    char c = (char) getc(fp);

    for (int i = 0; i < files_to_monitor; i++) {
        // buffers to keep temporary data
        char *path_buffor = calloc(2048, sizeof(char));
        char *number_buffor = calloc(100, sizeof(char));

        int k = 0 ;

        while (c != EOF && c != ' ') {
            path_buffor[k] = c;
            c = (char)getc(fp);
            k++;
        }
        while (c == ' ') {
            c = (char)getc(fp);
        }
        index = 0;

        while((int)c >= '0' && (int)c <= '9'){
            number_buffor[index] = c;
            c = (char)getc(fp);
            index++;
        }
        if(c == '\n'){
            c = (char)getc(fp);
        }
        result->path[i] = path_buffor;
        result->freq_time[i] = (int) strtol(number_buffor,NULL, 10);

        free(number_buffor);
    }

    fclose(fp);
    result->files_to_monitor = files_to_monitor;

    return result;

}

char * load_file_to_memory(FILE *fp){
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);

    fseek(fp, 0, SEEK_SET);
    char *buffer = malloc(fsize + 1);
    fread(buffer, fsize, 1, fp);
    return buffer;
}

void stop_handler(int sig_num) {
    process_stopped = 1;
}
void start_handler(int sig_num) {
    process_stopped = 0;
}
void end_handler(int sig_num) {
    killed = 1;
}



void monitor(char *path, unsigned int monitor_freq){

    FILE *fp = fopen(path, "r+");
    FILE *copy;
    int result = 0;

    struct sigaction stop_action;
    struct sigaction start_action;
    struct sigaction end_action;

    stop_action.sa_handler = stop_handler;
    stop_action.sa_flags = 0;

    start_action.sa_handler = start_handler;
    start_action.sa_flags = 0;

    start_action.sa_handler = end_handler;
    start_action.sa_flags = 0;



    struct stat file_stat;

    stat(path, &file_stat);


    // load file into memory
    char *buffer = load_file_to_memory(fp);
    fclose(fp);
    time_t last_mod = file_stat.st_mtime;



    while(!killed) {
        if(!process_stopped) {
            stat(path, &file_stat);
            if (file_stat.st_mtime > last_mod) {

                copy = fopen(get_file_name(file_stat.st_mtime, path), "w+");

                if (fputs(buffer, copy) == EOF) {
                    printf("Error while writing to file.");
                    exit(0);
                }

                fclose(copy);
                free(buffer);

                fopen(path, "r+");
                buffer = load_file_to_memory(fp);
                fclose(fp);

                last_mod = file_stat.st_mtime;
                result++;
            }
            sleep(monitor_freq);
        }
        sigaction(SIGUSR2, &stop_action, NULL);
        sigaction(SIGUSR1, &start_action, NULL);
        sigaction(SIGINT, &end_action, NULL);
    }

}




void create_archive(char * path){
    char *full_path = calloc(1024, sizeof(char));
    strcat(full_path, path);
    strcat(full_path, "/archive");
    DIR* dir = opendir(full_path);
    if (dir)
        closedir(dir);
    else if (ENOENT == errno){
        char *command=calloc(1024, sizeof(char));
        strcat(command, "mkdir ");
        strcat(command, full_path);
        system(command);
        free(command);
    }
    free(full_path);
}

void list(int *pid, const int *active, char **path, int n){

    for (int i=0; i<n; i++){
        if(active[i])
            printf("PID: %d  FILE: %s\n", pid[i], path[i]);
    }
}


int main(int argc, char **argv) {

    if(argc < 1){
        printf("Not enough arguments");
        return 1;
    }
    struct Files *files = parse(argv[1]);

    create_archive(get_directory_path(argv[1]));
    printf("%s\n", argv[1]);

    pid_t pid[files->files_to_monitor];

    int active[files->files_to_monitor];

    int stat;

    for(int i = 0; i < files->files_to_monitor; i++){
        pid[i] = fork();
        active[i] = 1;
        if(pid[i] == 0){
                monitor(files->path[i], (unsigned) files->freq_time[i]);
            }
    }

    list(pid, active, files->path, files->files_to_monitor);

    char *buf = malloc(20 * sizeof(char));
    int input_pid;

    while(1){
        scanf("%s", buf);


        if(strcmp(buf, "LIST") == 0){
            list(pid, active, files->path, files->files_to_monitor);
        }
        else if(strcmp(buf, "STOP") == 0){
            scanf("%d", &input_pid);
            for(int i = 0; i < files->files_to_monitor; i++)
                if(pid[i] == input_pid){
                    kill(pid[i], SIGUSR2);
                    active[i] = 0;
                }
        }
        else if(strcmp(buf, "START") == 0){
            scanf("%d", &input_pid);
            for(int i = 0; i < files->files_to_monitor; i++)
                if(pid[i] == input_pid){
                    kill(pid[i], SIGUSR1);
                    active[i] = 1;
                }
        }
        else if(strcmp(buf, "STARTALL") == 0){
            for(int i = 0; i < files->files_to_monitor; i++){
                    kill(pid[i], SIGUSR1);
                    active[i] = 1;
                }
        }  else if(strcmp(buf, "STOPALL") == 0){
            for(int i = 0; i < files->files_to_monitor; i++) {
                kill(pid[i], SIGUSR2);
                active[i] = 0;
            }

        }
        else if(strcmp(buf, "END") == 0){
            for(int i = 0; i < files->files_to_monitor; i++)
                kill(pid[i], SIGINT);
            break;
        }
    }


    for (int i=0; i<files->files_to_monitor; i++){
        waitpid(pid[i], &stat, 0);
        printf("Proces PID: %d utworzył %d kopii pliku\n",
                pid[i], WEXITSTATUS(stat));
    }

    return 0;
}
