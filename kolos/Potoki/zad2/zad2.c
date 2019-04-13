#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>

/// Za pomoca potoku wyslij wiadomosc do potomka, nastpęnie odczytaj ją z potoku
// nienazwanego i odeslij mu wiadomosc z kwadratem tej zmiennej,
// zwróconą zmienna wypisz na ekran

int main(int argc, char* argv[])
{

 if(argc !=2){
    printf("Not a suitable number of program parameters\n");
    exit(1);
 }

 int toChild[2];
 int toParent[2];
 int val1, val2, val3;
 val1 = atoi(argv[1]);
 printf("%d\n", val1 );
 pipe(toChild);
 pipe(toParent);
 int child = fork();

 if(child == 0){
   close(toChild[1]);
   close(toParent[0]);

   read(toChild[0], &val2, sizeof(int));

   val2 = val2*val2;

   write(toParent[1], &val2, sizeof(int));

 }else{
   close(toParent[1]);
   close(toChild[0]);

   write(toChild[1], &val1, sizeof(int));
   read(toParent[0], &val3, sizeof(int));

   printf("%d\n",  val3 );
   wait(NULL);
 }

}

//odczytaj z potoku nienazwanego wartosc przekazana przez proces potomny i zapisz w zmiennej val3
