
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

#define BUFF_SIZE 512
#define BLOCK "block"
#define INTERLEAVED "interleaved"

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })



void read_image(char *path);
void *interleaved_thread_start(void *arg);
void *block_thread_start(void *arg);
void write_results(char *path, char *to_write);
void save_result_picture( char *path);
int calculate_pixel(int x, int y);
void read_filter(char *file_path);
