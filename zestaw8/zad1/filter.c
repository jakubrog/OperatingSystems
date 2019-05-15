#include "filter.h"


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

    printf("%d %d\n",height, width );
    printf("Here is ok\n");

    double picture[height][width];

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
    result.width = x;
    result.height = y;
    result.image = picture;

    return result;
}

void *thread_start(void *arg){
    printf("Works");
}


int single_pixel(int x, int y, struct image pictrue, struct image filter) {
    double result = 0;
    int c = filter.height;
    int _x, _y;
    for(int i = 0; i < c; i++)
        for(int j = 0; j < c; j++){
          _y = max(1, x-ceil((double)c/2) + i);
          _x = max(1, y-ceil((double)c/2) + j);
          result += pictrue.image[_y][_x] * filter.image[i][j];
    }
    return (int)round(result) % 255;
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
    t_info->mode = argv[4]; // BLOCK or INTERLEAVED

    double result[t_info->threads_no];

    pthread_t thread[t_info->threads_no];
    struct thread_arg t_arg[t_info->threads_no];

    for(int i = 0; i< t_info->threads_no; i++){
       t_arg[i].thread_no = i;
       t_arg[i].info = t_info;
    }

    for(int i = 0; i < t_info->threads_no; i++){
        pthread_create(&thread[i], NULL, thread_start, &t_arg[i]);
    }



    // for(int i = 0; i < t_info->threads_no; i++){
    //     errno = pthread_join(&thread[i], (void**)&result);
    //     if (s != 0)
    //         handle_error_en(s, "pthread_join");
    // }
    return 0;
}
