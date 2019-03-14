#include <time.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <limits.h>
#include <ftw.h>

 urządzenie blokowe - block dev, potok nazwany - fifo, link symboliczny - slink, soket - sock)

const char date_format[] = "%Y-%m-%d %H:%M:%S";

double date_compare(time_t date_1, time_t date_2) {
    return difftime(date_1, date_2);
}

print_file_stat(struct stat file_stat, char *path){
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
    printf("",st_mtime; /* czas ostatniej modyfikacji */
    time_t st_ctime;


}

void read_dir(const char *path, int comp_mode, time_t comp_date){

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

            if(S_ISDIR(file_stat.st_mode)){  // check whether file is directory, if so go into it. It uses st_mode
                read_dir(abs_path, comp_mode, comp_date);
            }

            else if (S_ISREG(file_stat.st_mode)){  // is regular file ??
                if( (comp_mode == -1 && comp_date < file_stat.st_mtime) ||
                    (comp_mode == 0 && comp_date == file_stat.st_mtime) ||
                    (comp_mode == 1 && comp_date > file_stat.st_mtime)){


                    print_file_stat(file_stat,abs_path);
                }
            }
        }

        file = readdir(dir);  // it reads file, and moving cursor to next file, if null then it is not more files to read
    }

    closedir(dir);
}
int main() {
    printf("Hello, World!\n");
    return 0;
}