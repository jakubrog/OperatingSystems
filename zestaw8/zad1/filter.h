#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <cstdio>
#include <unistd.h>
#include <ctype.h>

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct image{
  int **image;
  int weight;
  int height;
}

struct thread_info{
  int threads_no;
  image input;
  image result;
  image filter;
}



image read_image(char *path);
void *thread_start(void *arg);
void elapsed_time(clock_t stat, clock_t end);
void write_results(char *path, char *to_write);
