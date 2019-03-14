#define _XOPEN_SOURCE 500   // dont know whats that but without this it doesnt work :o
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <limits.h>
#include <ftw.h>
#include <time.h>
//urządzenie blokowe - block dev, potok nazwany - fifo, link symboliczny - slink, soket - sock)

const char date_format[] = "%Y-%m-%d %H:%M:%S";
time_t global_time;  // global argument for nftw
char *sign_global;

double date_compare(time_t date_1, time_t date_2) {
    return difftime(date_1, date_2);
}

void print_file_stat(struct stat file_stat,const char *path){
    printf("%s\n", path);

    if(S_ISREG(file_stat.st_mode)){  // regular file?
        printf("file\n");
    }else if(S_ISDIR(file_stat.st_mode)){  //directory?
        printf("directory\n");
    }else if(S_ISCHR(file_stat.st_mode)){ // char device?
        printf("char dev\n");
    }else if(S_ISBLK(file_stat.st_mode)){
        printf("block dev\n");
    }else if(S_ISFIFO(file_stat.st_mode)){
        printf("fifo\n");
    }else if(S_ISLNK(file_stat.st_mode)){
        printf("fifo\n");
    }else if(S_ISSOCK(file_stat.st_mode)){
        printf("sock");
    }
    printf("Size: %ld\n",file_stat.st_size);
    printf("Last access: %ld\n",file_stat.st_atime);
    printf("Last modiified: %ld\n",file_stat.st_mtime);


}


int print_nftw(const char *fpath, const struct stat *file_stat, int flag, struct FTW *ftw) {

    if ((strcmp(sign_global, ">") == 0 && global_time < file_stat->st_mtime) ||
    ((strcmp(sign_global, "=") == 0) && global_time  == file_stat->st_mtime) ||
    ((strcmp(sign_global, "<") == 0) && global_time  > file_stat->st_mtime)) {

        print_file_stat(*file_stat,fpath);

    }
}

void read_dir(const char *path, int comp_mode, time_t usr_date){

    DIR *dir = opendir(path);
    if(dir == NULL){
        puts("Open dir failed!");

    }

    struct dirent *file = readdir(dir);  // struct keeps information about file (inode, name)

    while(file != NULL){

        char abs_path[1024];  // absolute path to file

        // creating path to file
        strcpy(abs_path, path);
        strcat(abs_path, "/");
        strcat(abs_path, file -> d_name);

        // chech whether file not keeps info about current directory and parent directory
        if(strcmp(file -> d_name, ".") != 0 && strcmp(file -> d_name, "..") != 0){

            struct stat file_stat;   // file stats struct

            stat(abs_path, &file_stat); // load info about file


            if (S_ISREG(file_stat.st_mode))  // is regular file ??
                if( (comp_mode == -1 && usr_date < file_stat.st_mtime) ||
                    (comp_mode == 0 && usr_date == file_stat.st_mtime) ||
                    (comp_mode == 1 && usr_date > file_stat.st_mtime)){

                    print_file_stat(file_stat,abs_path);

                }
                else if(S_ISDIR(file_stat.st_mode))  // check whether file is directory, if so go into it. It uses st_mode
                    read_dir(abs_path, comp_mode, usr_date);


        }

        file = readdir(dir);  // it reads file, and moving cursor to next file, if null then it is not more files to read
    }

    closedir(dir);
}


int main(int argc, char **argv) {
    if(argc < 4) {
        printf("Not enough arguments.");
        return 1;
    }

    char *path = argv[1];
    char *sign = argv[2];
    char *usr_date = argv[3];


    struct tm *date = malloc(sizeof(struct tm));
    strptime(usr_date, date_format, date); // saving user date into tm struct (date)

    time_t time = mktime(date); // getting latest time, need for comparing


    DIR *dir = opendir(path);
    if(!dir){
        printf("Wrong path");
        return 1;
    }
    if(strcmp(sign, "=") == 0){
        read_dir(path,0,time);
    }else if(strcmp(sign, ">") == 0){
        read_dir(path,1,time);
    }else if(strcmp(sign, "<") == 0){
        read_dir(path,0,time);
    }

    global_time = time;
    sign_global = sign;


    nftw(path, print_nftw, 10, FTW_PHYS); // FTW_PHYS - perform a physical walk and shall not follow symbolic links


    return 0;
}
