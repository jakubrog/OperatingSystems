#include "filter.h"


void save_picture(int w, int h, int **out_pict_matrix, char *path) {
    printf("hereisok\n");
    FILE *fp = fopen(path, "w+");
    if(!fp){
      printf("Cannot open file %s\n", path );
      exit(1);
    }
    char buff[1024];
    fprintf(fp, "P2\n");
    fprintf(fp, "%d %d\n", w, h);
    fprintf(fp, "%d\n", 255);

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            if (j < w - 1) {
                sprintf(buff, "%d ", out_pict_matrix[i][j]);
            } else {
                sprintf(buff, "%d\n", out_pict_matrix[i][j]);
            }
            fputs(buff, fp);
        }
    }
    fclose(fp);
}


struct image read_image(char *path){

    struct image result;

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
    printf("h = %d w = %d\n",height, width);

    double **picture = calloc(height, sizeof(double*));
    for(int i = 0; i< height; i++)
      picture[i] = calloc(width, sizeof(double));

    fgets(buff, BUFF_SIZE, file_in); // skip line

    int x = 0;
    int y = 0;
    while (fgets(buff, BUFF_SIZE, file_in) != NULL) {
        char *number = strtok(buff, " \n\t\r");
        while (number != NULL) {
            picture[y][x] = (double)strtol(number, NULL, 10);
            x++;
            if (x == width) {
                y++;
                x = 0;
            }
            number = strtok(NULL, " \t\n\r");
        }
    }

    fclose(file_in);
    result.width = width;
    result.height = height;
    result.image = picture;

    return result;
}

clock_t sub_time(clock_t start, clock_t end) {
    return (end - start) / CLOCKS_PER_SEC;
}

void *thread_start(void *arg){

    struct thread_arg *t_arg = (struct thread_arg*) arg;
    struct thread_info *info = t_arg->info;

    int k = t_arg->thread_no;
    int m = info->threads_no;
    int n = info->input.width;
    int x, y;
    clock_t start, end;
    start = clock();
     for( x = k ; x < k + 1 ; x++)
       for(y = 0 ; y < info->input.height; y++){
          info->result_image[y][x] = 200;
    }
    end = clock();
    return (end - start) / CLOCKS_PER_SEC;

}


int single_pixel(int x, int y, struct image pictrue, struct image filter) {
    double result = 0;
    int c = filter.height;
    int _x, _y;
    /*
    for(int i = 0; i < c; i++)
        for(int j = 0; j < c; j++){
          _y = max(1, x-ceil((double)c/2) + i);
          _x = max(1, y-ceil((double)c/2) + j);
          result += pictrue.image[_y][_x] * filter.image[i][j];
    }
    */

    return (int)round(pictrue.image[y][x] * 0.5) % 255;
}


int main(int argc, char **argv){

    int errno;
    if(argc < 2){
        handle_error("Not enough no. of arguments");
    }

    struct thread_info *t_info = malloc(sizeof(struct thread_info));
    t_info->threads_no = atoi(argv[1]);
    t_info->input = read_image(argv[2]);
    t_info->filter = read_image(argv[3]);


    int **result = calloc(t_info->input.height, sizeof(int*));
    for(int i = 0; i< t_info->input.height; i++)
       result[i] = calloc(t_info->input.width, sizeof(int));

    t_info->result_image = result;
    t_info->mode = argv[4]; // BLOCK or INTERLEAVED

    double time_results[t_info->threads_no];

    pthread_t thread[t_info->threads_no];
    struct thread_arg t_arg[t_info->threads_no];

    for(int i = 0; i< t_info->threads_no; i++){
       t_arg[i].thread_no = i;
       t_arg[i].info = t_info;
    }

    clock_t start, end;
    start = clock();

    for(int i = 0; i < t_info->threads_no; i++){
        pthread_create(&thread[i], NULL, thread_start, &t_arg[i]);
        printf("creating %d\n",i );
        usleep(10);
    }
    for(int i = 0; i<t_info->input.height; i++ ){
      for(int j = 0; j< t_info->input.width;j++)
          printf("%d ", t_info->result_image[i][j]);
      printf("\n");
    }
  //  save_picture(t_info->input.height, t_info->input.width, t_info->result_image, "test.pgm");
    sleep(2);
    end = clock();

    for(int i = 0; i < t_info->threads_no; i++){
        pthread_join(thread[i], (void**)&time_results[i]);
        printf("Thread no. time: %ld\n", time_results[i]);
    }

    printf("Main time: %ld \n", (end-start));
    return 0;
}
