#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define BUFF_SIZE 512

#define BLOCK "block"
#define INTERLEAVED "interleaved"
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

struct thread_arg{
  int thread_no;
  struct thread_info *info;
} thread_arg;

struct image{
  double **image;
  int width;
  int height;
}image;

struct thread_info{
  int threads_no;
  struct image input;
  struct image result;
  struct image filter;
  char *mode;
}thread_info;


int single_pixel(int x, int y, struct image pictrue, struct image filter);
struct image read_image(char *path);
void *thread_start(void *arg);
void elapsed_time(clock_t stat, clock_t end);
void write_results(char *path, char *to_write);
