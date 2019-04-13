#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	 int pdesk[2];

	if (pipe(pdesk) == -1){
		perror("PIPE");
		return 1;
	}



switch(fork()){
	  case -1:
		perror("FORK");
		return 2;
	  case 0:
				close(pdesk[0]);
				dup2(pdesk[1], STDOUT_FILENO);
				execvp("ls", argv);
				perror("EXECVP ls");
				exit(1);
	  default: {
			sleep(1);
			close(pdesk[1]);
			char* arg[] = {"tr", "a-z","A-Z", NULL};
			dup2(pdesk[0], STDIN_FILENO);
			execvp("tr",arg);

		}
}
   return 0;
}
