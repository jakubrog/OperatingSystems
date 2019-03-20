#define _XOPEN_SOURCE 500

#include<stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>



const char date_format[] = "%Y-%m-%d %H:%M:%S";


void print_dir_stat(const char *path) {
    printf("\n\nPID = %d,   %s\n",(int)getppid(), path);

    if(fork() == 0) {
        execlp("ls", "ls", "-l",NULL);
        exit(0);
    }else{
        wait(NULL);
    }
}


void read_dir(const char *path, int comp_mode, time_t usr_date) {

    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("%s open failed", path);
        puts("error");
    }

    struct dirent *file = readdir(dir);  // struct keeps information about file (inode, name)
    char abs_path[1024]; // absolute path to file

    while (file != NULL) {


        // creating path to file
        strcpy(abs_path, path);
        strcat(abs_path, "/");

        strcat(abs_path, file->d_name);

        struct stat file_stat;// file stats struct
        lstat(abs_path, &file_stat); // load info about file

        // chech whether file not keeps info about current directory and parent directory
        if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {


            if (S_ISDIR(file_stat.st_mode)) {
                // check whether file is directory, if so go into it
                if ((fork()) == 0) {
                    print_dir_stat(abs_path);
                    read_dir(abs_path, comp_mode, usr_date);
                    exit(0);
                }else{
                    wait(NULL); //PARENT WAITS
                }
            }
        }
        // it reads file, and moving cursor to next file, if null then it is not more files to read
        file = readdir(dir);
    }
    closedir(dir);
}


int main(int argc, char **argv) {
    if (argc < 4) {
        printf("Not enough arguments.");
        return 1;
    }

    char *path = argv[1];
    char *sign = argv[2];
    char *usr_date = argv[3];


    struct tm *date = malloc(sizeof(struct tm));
    strptime(usr_date, date_format, date); // saving user date into tm struct (date)

    time_t time = mktime(date); // getting latest time, need for comparing
    char *r_path = realpath(path, NULL);

    DIR *dir = opendir(r_path);

    if (!dir) {
        printf("Wrong path");
        return 1;
    }

    if (strcmp(sign, "=") == 0) {
        read_dir(r_path, 0, time);
    } else if (strcmp(sign, "<") == 0) {
        read_dir(r_path, 1, time);
    } else if (strcmp(sign, ">") == 0) {
        read_dir(r_path, -1, time);
    }


    return 0;
}
