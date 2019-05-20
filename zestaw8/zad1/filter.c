#include "filter.h"


int **image = NULL;
int i_height;
int i_width;

double **filter = NULL;
int f_size;
int **result = NULL;

int thread_count;
int *thread_no;


void clear(){
  int i;
    if(image != NULL){
        for(i = 0; i < i_height; i++)
            free(image[i]);
        free(image);
    }
    if(result != NULL){
        for(i = 0; i < i_height; i++)
          free(result[i]);
        free(result);
    }
    if(filter != NULL){
      for(i = 0; i < f_size; i++)
          free(filter[i]);
      free(filter);
  }
}

int calculate_pixel(int x, int y){
    int i,j;
    double sum = 0;
    for(i = 0; i < f_size; i++){
        for(j = 0; j < f_size; j++){
            int h = max(0, x - ceil(f_size / 2.0) + i);
            int w = max(0, y - ceil(f_size / 2.0) + j);
            sum += h < i_height && w < i_width ? image[h][w] * filter[i][j] : 0;
        }
    }
    return (int)round(sum);
}

void print_image(int **array, int w, int h){
      int index = 0;
      for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
          printf("%d  ", array[i][j]);
          index++;
          if(index >= 70){
            printf("\n");
            index = 0;
          }
        }
      }
}

void *block_thread_start(void *arg){
    int *no = (int*)arg;
    int k = no[0];

    double *ret_val = malloc(sizeof(double));
    int x_min = (k-1) * ceil(i_width / (1.0*thread_count));
    int x_max = (k) * ceil(i_width / (1.0*thread_count)) - 1;

    clock_t t;
    t = clock();

    for(int i = x_min; i <= x_max; i++){
           for(int j = 0; j < i_height; j++){
               result[j][i] = calculate_pixel(i,j);
           }
     }

    t = clock() - t;
    *ret_val = ((double)t)/CLOCKS_PER_SEC;
    pthread_exit(ret_val);
}


void *interleaved_thread_start(void *arg){
  int *no = (int*)arg;
  int k = no[0];

  double *ret_val = malloc(sizeof(double));

  clock_t t;
  t = clock();

  for(int i = k-1; i < i_width; i += k){
     for(int j = 0; j < i_height; j++){
        result[j][i] = calculate_pixel(i, j);
     }
 }

  t = clock() - t;
  *ret_val = ((double)t)/CLOCKS_PER_SEC;
  pthread_exit(ret_val);
}

int main(int argc, char **argv){
    atexit(clear);

    if(argc < 6){
        printf("Not enough arguments");
        exit(1);
    }

    thread_count = atoi(argv[1]);

    read_image(argv[2]);
    read_filter(argv[3]);

    result = calloc(i_height, sizeof(int*));

    for(int i = 0; i< i_height; i++)
       result[i] = calloc(i_width, sizeof(int));


    double *time_results[thread_count];
    thread_no = malloc(thread_count*sizeof(int));
    for(int i = 0; i < thread_count; i++)
        thread_no[i] = i + 1;

    pthread_t thread[thread_count];

    clock_t t;

    t = clock();

    if(strcmp(argv[4], BLOCK) == 0){
        for(int i = 0; i < thread_count; i++){
            pthread_create(&thread[i],NULL,block_thread_start, &thread_no[i]);
        }
    }else if(strcmp(argv[4], INTERLEAVED) == 0){
        for(int i = 0; i < thread_count; i++){
            pthread_create(&thread[i],NULL,interleaved_thread_start, &thread_no[i]);
        }
    }else{
      printf("Wrong mode\n");
      exit(1);
    }

    save_result_picture(argv[5]);

    for(int i = 0; i < thread_count; i++){
      pthread_join(thread[i], (void*)&time_results[i]);
    }

    t = clock() - t;
    printf("Mode: %s\n", argv[4]);
    printf("No of threads: %d\n", thread_count);
    printf("Filter dimensions: %d x %d\n", f_size, f_size);
    printf("Image dimensions: %d x %d\n", i_height, i_width);
    printf("Summary time: %fs\nThreads times:\n", ((double)t)/CLOCKS_PER_SEC);
    for(int i = 0; i < thread_count; i++){
      printf("\t%d. %fs\n",i+1, *time_results[i] );
    }
    printf("\n\n\n");

    return 0;
}


void read_image(char *path){
    FILE *file_in = fopen(path, "r+");

    if(!file_in){
        printf("Cannot open file %s\n", path);
        exit(1);
    }

    char buff[BUFF_SIZE];
    fgets(buff, BUFF_SIZE, file_in); // skip first line
    fgets(buff, BUFF_SIZE, file_in); // load dimensions

    char *dimensions;

    dimensions = strdup(buff);
    int width =  (int) strtol(strsep(&dimensions, " \t"), NULL, 10);
    int height = (int) strtol(strsep(&dimensions, " \t"), NULL, 10);
    int **picture = calloc(height, sizeof(int*));

    for(int i = 0; i< height; i++)
      picture[i] = calloc(width, sizeof(int));

    fgets(buff, BUFF_SIZE, file_in); // skip line

    int x = 0;
    int y = 0;
    while (fgets(buff, BUFF_SIZE, file_in) != NULL) {
        char *number = strtok(buff, " \n\t\r");
        while (number != NULL) {
            picture[y][x] = strtol(number, NULL, 10);
            x++;
            if (x == width) {
                y++;
                x = 0;
            }
            number = strtok(NULL, " \t\n\r");
        }
    }

    fclose(file_in);
    i_height = height;
    i_width = width;
    image = picture;
}


void read_filter(char *path){
  FILE *file_in = fopen(path, "r+");

  if(!file_in){
      printf("Cannot open file %s\n", path);
      exit(1);
  }

  char buff[BUFF_SIZE];
  fgets(buff, BUFF_SIZE, file_in); // skip first line
  fgets(buff, BUFF_SIZE, file_in); // load dimensions

  char *dimensions;

  dimensions = strdup(buff);
  int width =  (int) strtol(strsep(&dimensions, " \t"), NULL, 10);
  int height = (int) strtol(strsep(&dimensions, " \t"), NULL, 10);
  double **picture = calloc(height, sizeof(double*));

  for(int i = 0; i< height; i++)
    picture[i] = calloc(width, sizeof(double));

  fgets(buff, BUFF_SIZE, file_in); // skip line

  int x = 0;
  int y = 0;
  while (fgets(buff, BUFF_SIZE, file_in) != NULL) {
      char *number = strtok(buff, " \n\t\r");
      while (number != NULL) {
          picture[y][x] = atof(number); // atof - convert string to double
          x++;
          if (x == width) {
              y++;
              x = 0;
          }
          number = strtok(NULL, " \t\n\r");
      }
  }

  fclose(file_in);
  f_size = width;
  filter = picture;

}

void save_result_picture(char *path) {
    FILE *fp = fopen(path, "w+");
    if(!fp){
      printf("Cannot open file %s\n", path );
      exit(1);
    }
    char buff[1024];
    fprintf(fp, "P2\n");
    fprintf(fp, "%d %d\n", i_width, i_height);
    fprintf(fp, "%d\n", 255);

    for (int i = 0; i < i_height; i++) {
        for (int j = 0; j < i_width; j++) {
            if (j < i_width - 1) {
                sprintf(buff, "%d ", result[i][j]);
            } else {
                sprintf(buff, "%d\n", result[i][j]);
            }
            fputs(buff, fp);
        }
    }
    fclose(fp);
}
