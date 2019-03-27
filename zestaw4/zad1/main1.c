#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

void handler2() {
    _exit(2);
}

void handler() {
    printf("Ctrl+Z to close");

    signal(SIGTSTP, handler2);
}



int main()
{
   time_t t = time(NULL);

   while(1){
     struct tm tm = *localtime(&t);
     printf("%d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
     signal(SIGTSTP, handler);
     pause();
     sleep(1);
   }
   return 0;
}
