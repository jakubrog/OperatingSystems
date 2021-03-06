#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/types.h>


#define LINES_MAX 1000
#define COMANDS_MAX 100

char **parse_arguments(char *line);
int line_exec(char *command);


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("%s", "You need to specify file with commands.");
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");

    if (!fp) {
        printf("%s", "File cannot be opened");
        return 1;
    }

    char command[LINES_MAX];
    pid_t pid;
    while (fgets(command, LINES_MAX, fp) != NULL) {    // read one line from file to temp_registry, of max size LINES_MAX
        pid = fork ();
        if (pid == 0) {
            line_exec(command); // every line is executed by different process
            exit(0);
        }else{
          wait(NULL);
        }
    }
    fclose(fp);
    return 0;
}

// given result[0] is program to launch and the rest are arguments
char **parse_arguments(char *line) {
    int size = 0;
    char **args = NULL;

    char delimiters[3] = {' ', '\n', '\t'};
    char *a = strtok(line, delimiters);

    while (a != NULL) {
        size++;

        args = realloc(args, sizeof(char *) * size);

        if (args == NULL) {
            exit(1);   //realocating failed
        }
        args[size - 1] = a;

        a = strtok(NULL, delimiters);
    }

    args = realloc(args, sizeof(char *) * (size + 1));

    if (args == NULL) {
        exit(2);  //realocating failed
    }

    args[size] = NULL;

    return args;
}


int line_exec(char *command) {

    int command_number = 0;

    // int pipes[2][2]; // only two is ok b
    char *commands[COMANDS_MAX];  // array keeps commands between pipes, commands separated with pipe are located in
    // different blocks ex. ls | grep a => commands[0]= "ls " , commands[1] = " grep a"

    commands[0] = strtok(command, "|");

    while (commands[command_number] != NULL) {
        command_number++;
        commands[command_number] = strtok(NULL, "|");
    };
    int pipes[command_number+1][2];
    int i;
    for (i = 0; i < command_number; i++) {

        if (i != 0) {
            close(pipes[i-1][0]);  // if it's not first loop close descriptors used before
            close(pipes[i-1][1]);
        }

        if (pipe(pipes[i % 2]) == -1) {
            printf("Error on pipe.\n");
            exit(3);
        }

        pid_t pid = fork();

        if (pid == 0) {

            char **exec_params = parse_arguments(commands[i]);

            if (i != command_number - 1) {
                close(pipes[i][0]);
                if (dup2(pipes[i][1], STDOUT_FILENO) < 0) {
                    exit(4);
                }
            }

            if (i != 0) {
                close(pipes[(i + 1)][1]);
                if (dup2(pipes[(i + 1)][0], STDIN_FILENO) < 0) {
                    exit(5);
                }

            }
            execvp(exec_params[0], exec_params);  //execvp(const char *file, char *const argv[]);

            exit(0);
        }
    }

    close(pipes[command_number][0]);
    close(pipes[command_number][1]);
    wait(NULL);
    exit(0);
}
