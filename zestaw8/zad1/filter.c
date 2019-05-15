#include "filter.h"

void *thread_start(void *arg){

}

int main(int argc, int *argv){
  if(argc < 5){
    handle_error("Not enough no. of arguments");
  }

  thread_info *t_info;
  t_info->threads_no = atoi(argv[1]);
  t_info->input = read_image(argv[2]);
  t_info->filter = read_image(argv[3]);

  pthread_t thread[t_info->threads_no];

  for(int i = 0; i < t_info->threads_no; i++0){
     pthread_create(&thread[i], NULL, thread_start , &t_info);
  }
  int result;
  for(int i = 0; i < t_info->threads_no; i++0){
      result = pthread_join(&thread[i], NULL);
  }
  return 0;
}
