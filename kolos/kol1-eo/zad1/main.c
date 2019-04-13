#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char* argv[]) {
  DIR * katalog;
  if (argc!=2)
  {
    printf ("Wywołanie %s ścieżka",argv[0]);
    return 1;
  }
  struct dirent * pozycja;
  katalog = opendir(argv[1]);
  if(!katalog){
    printf("%s\n",strerror(errno));
    return 1;
  }
  pozycja  = readdir(katalog);
  while(pozycja != NULL){
    if(strcmp(pozycja->d_name, ".") != 0 && strcmp(pozycja->d_name, "..") != 0){
      printf("%s\n", pozycja->d_name);
    }
    pozycja = readdir(katalog);
  }
  if(errno != 0){
    printf("%s\n", strerror(errno));
    return 2;
  }
  closedir(katalog);
  return 0;
}
/*Otwórz katalog, w przypadku błędu otwarcia zwróć błąd funkcji otwierającej i zwrócć 1.
Wyświetl zawartość katalogu katalog, pomijając "." i ".."
Jeśli podczas tej operacji wartość errno zostanie ustawiona, zwróć błąd funkcji czytającej oraz wartość 2. */

/*koniec*/
