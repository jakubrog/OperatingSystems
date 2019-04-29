#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <limits.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include "headers.h"

#define SIZE 1024

static struct mesg_buffer mesg;
static mqd_t server_queue_id = -1;
static mqd_t queue_id;
static int id;
static pid_t child = -1;
static int p[2];
static char CLIENT_NAME[32];

char buff[1024];

void delete_queue(){
  mq_close(server_queue_id);
  mq_close(queue_id);
  mq_unlink(CLIENT_NAME); // removes queue
}

void send_message(int priority){
  if(server_queue_id != -1){
    if(mq_send(server_queue_id,(const char*)&mesg,sizeof(mesg), priority)< 0){
      printf("ERROR: %s\n",strerror(errno));
      fflush(stdout);
    }
  }
}

void exit_function_parent(){
  close(p[0]);
  mesg.id = id;
  mesg.type=STOP;
  send_message(STOP_PRIOR);
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
  if(( server_queue_id = mq_open(SERVER_NAME, O_WRONLY)) < 0 ){
    printf("ERROR: %s",strerror(errno));
    exit(1);
  }
}


void set_up_own_queue(){
  struct mq_attr attr;

  attr.mq_flags = O_NONBLOCK;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = sizeof(mesg);
  attr.mq_curmsgs = 0;

  // set client name as /pid
  CLIENT_NAME[0] = '/';
  sprintf(&CLIENT_NAME[1],"%d",getpid());
  printf("My name is: %s\n",CLIENT_NAME );

  mq_unlink(CLIENT_NAME); // remove queue if exists

  // create queue like in server
  if(( queue_id = mq_open(CLIENT_NAME, O_RDONLY | O_CREAT | O_NONBLOCK, 0777, &attr)) == -1 ){
    printf("ERROR: %s", strerror(errno) );
    exit(-1);
  }
}

void init_mesg(){
  mesg.id = queue_id; // give server your queue id
  mesg.type = INIT;
  strcpy(mesg.mesg_text, CLIENT_NAME);

  send_message(INIT_PRIOR);

  // wait for returning message
  while(mq_receive(queue_id,(char*)&mesg,sizeof(mesg),NULL) == -1)

  if(mesg.id == -1){
    exit(-1);
  }
  id = mesg.id; // id given by server

  printf("I am registered as %d client\n",id );
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

            for(int i = 0 ; i < strlen(token); i++)
              printf("%c\n",token[i] );
            printf("%s\n", token );
/************************************/
/*** READ INPUT AND SEND MESSAGE *///

            if(strcmp(token,"LIST") == 0 ){
              token = strtok_r(the_rest," ",&the_rest);
              mesg.type = LIST;
              send_message(LIST_PRIOR);

            }else if(strcmp(token,"FRIENDS")==0){
              if(the_rest == NULL){
                mesg.mesg_text[0]='\0';
              }else{
                strcpy(mesg.mesg_text,the_rest);
              }
              mesg.type = FRIENDS;
              send_message(FRIENDS_PRIOR);

            }else if(strcmp(token,"ADD") == 0){
              if(strcmp(the_rest,"\0") == 0 || strcmp(the_rest,"\n") == 0   ){
                printf("Not enough arguments\n");
              }else{
                mesg.type = ADD;
                strcpy(mesg.mesg_text,the_rest);
                send_message(ADD_PRIOR);
              }
            }else if(strcmp(token,"DEL")==0){
              if(strcmp(the_rest,"\0") == 0 ||strcmp(the_rest,"\n") == 0   ){
                printf("Not enough arguments\n");
                fflush(stdout);
              }else{
                mesg.type = DEL;
                strcpy(mesg.mesg_text,the_rest);
                send_message(DEL_PRIOR);
              }

            }else if(strcmp(token,"2ALL")==0){
              mesg.type = ALL2;
              strcpy(mesg.mesg_text,the_rest);
              send_message(ALL2_PRIOR);

            }else if(strcmp(token,"2FRIENDS")==0){
              mesg.type = FRIENDS2;
              strcpy(mesg.mesg_text,the_rest);
              send_message(FRIENDS2_PRIOR);

            }else if(strcmp(token,"2ONE")==0){
              mesg.type = ONE2;
              strcpy(mesg.mesg_text,the_rest);
              send_message(ONE2_PRIOR);

            }else if(strcmp(token,"STOP")==0){
              exit(0);

            }else if(strcmp(token,"ECHO")==0){
              mesg.type = ECHO;
              strcpy(mesg.mesg_text,the_rest);
              send_message(ECHO_PRIOR);

            }else{
              printf("Wrong command\n");
              fflush(stdout);
            }
          }

/************************************/
/*RECEIVE AND PRINT MESSAGE*///

// receive message with highest prority and load it into mesg
        if(mq_receive(queue_id,(char*)&mesg,sizeof(mesg), NULL) != -1 ){
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
