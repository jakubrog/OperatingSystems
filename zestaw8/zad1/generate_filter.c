#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>


int main(int argc, char **argv){
  if(argc < 2){
    printf("Not enough arguments\n");
    exit(1);
  }
  FILE *fp = fopen(argv[1], "w+");
  int c = atoi(argv[2]);
  fprintf(fp, "P1\n%d %d\n255\n", c, c);

  for(int i = 0; i<c; i++){
    for(int j = 0; j<c; j++)
        fprintf(fp, "%f ", 1.0/(c*c));
    fprintf(fp,"\n" );
  }


  fclose(fp);
  return 0;
}
