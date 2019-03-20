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


////TODO: ADD COUMINICATION BETWEEN PROCESSES AND SOMETHING TO PARARELL



struct Files {
    char **path;   // array of paths to every file
    int *freq_time; // monitoring time to every file
    int files_to_monitor;  // number of files to monitor
};



/// elapsed_time - difference between start time and current time
double elapsed_time(time_t start) {
    return difftime(time(NULL), start);
}


/// get_directory_path - extracts directory path from full path
char *get_directory_path(char *fullpath) {
    char *e = strrchr(fullpath, '/');
    if (!e) {
        char *buf = strdup(fullpath);
        return buf;
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
    char *temp = calloc(3, sizeof(char));

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
    char **paths;
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

    paths = (char **) malloc((files_to_monitor) * sizeof(char *));

    rewind(fp);

    //length of every path
    size_t path_length[files_to_monitor];

    // number of bytes to move cursor when reading paths from file (due to integer at the end of the row)
    size_t buff[files_to_monitor];

    int index;

    char c = (char) getc(fp);

    for (int i = 0; i < files_to_monitor; i++) {
        path_length[i] = 0;
        // loop to measure length of every path
        while (c != EOF && c != ' ') {
            c = (char) getc(fp);
            path_length[i]++;
        }

        // alocating memory for paths
        if (c == ' ') {
            paths[i] = (char *) malloc((path_length[i]) * sizeof(char));
            c = (char) getc(fp);
        }

        buff[i] = 0;
        index = 1;

        // making array with freq_time, converting from char to int and measure length
        while (c != EOF && c != '\n') {
            result->freq_time[i] = result->freq_time[i] * index + ((int)c - '0');
            index*=10;
            buff[i]++;
            c = (char)getc(fp);
        }

    }

    // coming back to beginning of file
    rewind(fp);

    for (int i = 0;i < files_to_monitor; i++) {
        // reading paths from file to array
        if (path_length[i]  != fread(paths[i],sizeof(char), path_length[i] , fp)) {
            printf("Cannot read from file");
            exit(1);
        }
        // move cursor to next line
        fseek(fp, buff[i], SEEK_CUR);
    }

    fclose(fp);
    result->path = paths;
    result->files_to_monitor = files_to_monitor;

    return result;

}

void monitor_copy_type(char *path, int monitoring_time, unsigned int monitor_freq) {
    FILE *fp = fopen(path, "r+");

    int result = 0;

    struct stat file_stat;

    lstat(path, &file_stat);

    time_t start = time(NULL);

    time_t last_mod = 0;

    while (elapsed_time(start) < monitoring_time) {
        lstat(path, &file_stat);
        if (file_stat.st_mtime > last_mod) {
            if (fork() == 0) {
                execlp("cp", "cp", path, get_file_name(file_stat.st_mtime, path), NULL);
                exit(0);
            }else{
                wait(NULL);
            }
            last_mod = file_stat.st_mtime;
            result++;
        }
        sleep(monitor_freq);
    }
    exit(result);
}
char * load_file_to_memory(FILE *fp){
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);

    fseek(fp, 0, SEEK_SET);
    char *buffer = malloc(fsize + 1);
    fread(buffer, fsize, 1, fp);
    return buffer;
}


void monitor_second_type(char *path, int monitoring_time, unsigned int monitor_freq){

    FILE *fp = fopen(path, "r+");
    FILE *copy;

    int result = 0;

    struct stat file_stat;

    lstat(path, &file_stat);


    // load file into memory
    char *buffer = load_file_to_memory(fp);
    fclose(fp);
    time_t last_mod = file_stat.st_mtime;

    time_t start = time(NULL);



    while (elapsed_time(start) < monitoring_time) {
        lstat(path, &file_stat);
        if (file_stat.st_mtime > last_mod) {

            copy = fopen(get_file_name(file_stat.st_mtime, path), "w+");

            if(fputs(buffer, copy) == EOF){
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
    exit(result);
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


int main(int argc, char **argv) {

    if(argc < 3){
        printf("Not enough arguments");
        return 1;
    }
    struct Files *files = parse(argv[1]);

    create_archive(get_directory_path(argv[1]));

    for(int i = 0; i < files->files_to_monitor; i++){
        int pid = fork();
        if(pid == 0){
            monitor_copy_type(files->path[i], (int) strtol(argv[2],NULL, 10), (unsigned) files->freq_time[i]);
           // exit(0);
        }else{
            wait(NULL);
        }
    }


    return 0;
}