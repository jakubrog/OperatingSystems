#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/ipc.h>
#include <limits.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>

#include "headers.h"

#define SIZE 1024

static struct mesg_buffer mesg;
static int server_queue_id = -1;
static int queue_id;
static int id;
static pid_t child = -1;
static int p[2];

char buff[1024];

void delete_queue(){
  msgctl(queue_id, IPC_RMID, NULL); // removes queue
}

void send_message(){
  if(server_queue_id != -1){
    msgsnd(server_queue_id, &mesg, mesg_size(),0);
  }
}

void exit_function_parent(){
  close(p[0]);
  mesg.id = id;
  mesg.priority=STOP_PRIOR;
  mesg.type=STOP;
  send_message();
  delete_queue();

  if(child!=-1){
    kill(SIGKILL,child);
  }
    return;
}


void exit_function_child(){
  close(p[1]);

  return;
}


void set_up_server_queue_id(){
  const char *homedir;

  if ((homedir = getenv("HOME")) == NULL) {
      homedir = getpwuid(getuid())->pw_dir;
  }

  key_t key_own= ftok(homedir, SERVER_SEED);

  if(( server_queue_id = msgget(key_own, 0777 | IPC_CREAT )) == -1 ){
    exit(1);
  }
}


void set_up_own_queue(){

  if(( queue_id = msgget(IPC_PRIVATE, 0777 | IPC_CREAT )) == -1 ){
    exit(1);
  }
}

void init_mesg(){
  mesg.priority = INIT_PRIOR;
  printf("Connected to queue with id: %d\n", queue_id);
  mesg.id = queue_id; // give server your queue id
  mesg.type = INIT;

  send_message();

  // first 0 means that first message in queue is read
  // second 0 is a bit mask
  msgrcv(queue_id,&mesg, mesg_size(), 0, 0);

  if(mesg.id == -1){
    exit(-1);
  }
  id = mesg.id; // id given by server

  printf("I am registered as %d client\n ",id );
  fflush(stdout); // clear output buffer
}

void exit_handle(){
  exit(0);
}

void parent_read(){
    close(p[1]);
    signal(SIGINT,exit_handle);
    atexit(exit_function_parent); // set normal process termination

    // create new queue or open existing one, the same as server
    set_up_server_queue_id();
    // create private queue
    set_up_own_queue();
    init_mesg();

    while (1) {
        if(read(p[0], buff, SIZE) > 0){
            char* the_rest = buff;
            char delimiters[3] = {' ','\n','\t'};
            char * token = strtok_r(the_rest,delimiters, &the_rest);
            mesg.id = id; // id given by server

/************************************/
/*** READ INPUT AND SEND MESSAGE *///

            if(strcmp(token,"LIST") == 0 ){
              token = strtok_r(the_rest," ",&the_rest);
              mesg.priority = LIST_PRIOR;
              mesg.type = LIST;
              send_message();

            }else if(strcmp(token,"FRIENDS")==0){
              if(the_rest == NULL){
                mesg.mesg_text[0]='\0';
              }else{
                strcpy(mesg.mesg_text,the_rest);
              }
              mesg.priority = FRIENDS_PRIOR;
              mesg.type = FRIENDS;
              send_message();

            }else if(strcmp(token,"ADD")==0){
              if(strcmp(the_rest,"\0") == 0 ||strcmp(the_rest,"\n") == 0   ){
                printf("Not enough arguments");
              }else{
                strcpy(mesg.mesg_text,the_rest);
                mesg.priority = ADD_PRIOR;
                mesg.type = ADD;
                send_message();
              }

            }else if(strcmp(token,"DEL")==0){
              if(strcmp(the_rest,"\0") == 0 ||strcmp(the_rest,"\n") == 0   ){
                printf("Not enough arguments\n");
              }else{
                mesg.priority = DEL_PRIOR;
                mesg.type = DEL;
                strcpy(mesg.mesg_text,the_rest);
                send_message();
              }
            }else if(strcmp(token,"2ALL")==0){
              mesg.priority = ALL2_PRIOR;
              mesg.type = ALL2;

              strcpy(mesg.mesg_text,the_rest);
              send_message();

            }else if(strcmp(token,"2FRIENDS")==0){
              mesg.priority = FRIENDS2_PRIOR;
              mesg.type = FRIENDS2;
              strcpy(mesg.mesg_text,the_rest);
              send_message();
            }else if(strcmp(token,"2ONE")==0){

              mesg.priority = ONE2_PRIOR;
              mesg.type = ONE2;
              strcpy(mesg.mesg_text,the_rest);
              send_message();
            }else if(strcmp(token,"STOP")==0){
              exit(0);
            }else if(strcmp(token,"ECHO")==0){
              mesg.priority = ECHO_PRIOR;
              mesg.type = ECHO;
              strcpy(mesg.mesg_text,the_rest);
              send_message();
            }else{
              printf("Wrong command\n");
              fflush(stdout);
            }
        }

/************************************/
/*RECEIVE AND PRINT MESSAGE*///
// IPC_NOWAIT - if there isnt any message return error

// PRIORITY - msgtyp field, If msgtyp is less than 0, then the first message in the
//queue with the lowest type less than or equal to the absolute value of msgtyp will
//be read.
        if(msgrcv(queue_id,&mesg,mesg_size(), PRIORITY, IPC_NOWAIT) != -1 ){
          switch(mesg.type){
            case LIST :
              printf("LIST: %s\n",mesg.mesg_text);
              fflush(stdout);
            break;

            case ECHO:
              printf("ECHO: %s",mesg.mesg_text);
              fflush(stdout);
            break;

            case ALL2:
              printf("2ALL: %s",mesg.mesg_text);
              fflush(stdout);
            break;

            case ONE2:
              printf("2ONE: %s",mesg.mesg_text);
              fflush(stdout);
            break;

            case FRIENDS2:
              printf("2FRIENDS: %s",mesg.mesg_text );
              fflush(stdout);
            break;

            case STOP:
              exit(0);
            break;

            default:
               break;
          }
        }
    }
}

void child_write()
{

    close(p[0]);
    atexit(exit_function_child); // set normal process termination
    while(1){
         fgets(buff,SIZE, stdin); // read SIZE-1 chars from standard input

        if(buff[0]!='\n')
          write(p[1], buff, SIZE);

    }
}

int main(int argc, char** argv){

  // There has to be pipe, becouse fgets is blocking function and there will
  // be problems to receiving messages
  if (pipe(p) < 0)
      exit(1);

  //SETING p[0]
  if (fcntl(p[0], F_SETFL, O_NONBLOCK) < 0)
      exit(2);

  switch (child = fork()) {
    case -1:
        exit(3);

    case 0:
        child_write();
        break;

    default:
        parent_read();
        break;
  }

  return 0;
}
