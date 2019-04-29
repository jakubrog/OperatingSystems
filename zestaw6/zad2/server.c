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

#define CLIENT_NO 30

// one static message for whole program, easier to use in funcitions and there is no need for more
static struct mesg_buffer mesg;
static mqd_t queue_id = -1;

static int friends[CLIENT_NO][CLIENT_NO]; // every firend can have a list with friends

// arraty with clients, where every client has its own id. Under this id in array there is queue descriptor
static mqd_t client_list[CLIENT_NO];


/**************************
MESSAGE HANDLERS
**************************/

// normal process termination
void exit_function(){
  struct mesg_buffer exit_message;
  exit_message.type = STOP;

  for(int i = 0 ; i < CLIENT_NO ; i++){
    if(client_list[i] != -1){
      mq_send(client_list[i], (const char*)&exit_message, sizeof(mesg), STOP_PRIOR);
    }
  }

    if(queue_id != -1){
      mq_close(queue_id); // close queue
      mq_unlink(SERVER_NAME); // delete queue
    }
}

// remove client friend list
void delete_client_friends_list(int client_id){
  if(client_id >= 0 && client_id < CLIENT_NO)
    for(int i = 0 ; i < CLIENT_NO ; i++)
      friends[client_id][i] = 0;
}


void print_friend_list(int client_id){
  printf("CLIENT FRIENDS: %d\n",client_id );

  for(int i = 0 ; i < CLIENT_NO ; ++i){
    if(friends[client_id][i]==1)
      printf("%d ",i);
    }

    printf("\n");
}

// funciton creates char array with clients list
void list_clients(){
  char list[MESSAGE_SIZE];
  list[0] = '\0';
  char buff[14];

  for(int i = 0 ; i < CLIENT_NO; ++i)
    if(client_list[i] != -1){
      sprintf(buff,"%d ",i);
      strcat(list,buff);
    }

  strcpy(mesg.mesg_text,list);
}

/// simple funcition to send message to client
void send_message_to_client(int client_id, int priority){
  if(client_id < CLIENT_NO && client_list[client_id] != -1){
    printf("Message sent to: %d \n\n", (int)client_list[client_id]);

//int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio);
    mq_send(client_list[client_id],(char*)&mesg, sizeof(mesg_buffer), priority);
  }
}

// simple function to add client
int add_client(mqd_t queue_id){
  for( int i = 0 ; i < CLIENT_NO ; ++i)
    if(client_list[i] == (mqd_t)(-1)){
      client_list[i] = queue_id;
      return i;
    }

  return -1;
}

// Adding or deleting friends of given client, argumet mode declares adding
// or deleting mode of fucntion, if mode equals 'a' functions is adding to
// friends list, if mode equals 'd' funciton is deleting from friends list,
// otherwise function does nothing
void friend_list_handle(int client_id, char* str, char mode){
  char * ptr = str;
  char * token = strtok_r(ptr," ",&ptr);

  while(ptr!=NULL && token!=NULL ){
    if(strcmp(token,"\0")!= 0 && strcmp(token,"\n") != 0){
      int index = strtol(token,NULL,10); // load new friend id to index

      if(index >=0 && index < CLIENT_NO){
        if(mode == 'a')
          friends[client_id][index] = 1;
        if(mode == 'd')
          friends[client_id][index] = 0;
        }
    }
    token = strtok_r(ptr," ",&ptr); // move to next number
  }
}

// adding client to queue
void init_handle(){
  int client_id;
  mqd_t list = mq_open(mesg.mesg_text, O_WRONLY);
  if(list < 0){
    printf("ERROR: %s\n",strerror(errno));
    exit(-1);
  }
  if( (client_id = add_client(list)) == -1){
    printf("Cannot initialize client\n");
    return;
  }


  printf("Client no %d has been initialized\n", client_id);
  mesg.type = INIT;
  mesg.id = client_id;
  send_message_to_client(client_id, INIT_PRIOR);
}

void list_handle(){
  int client_id = mesg.id;
  printf("LIST from: %d\n",mesg.id);
  list_clients();
  mesg.type = LIST; /// need here?
  send_message_to_client(client_id, LIST_PRIOR);
}

// funciton send message with string, current time and sender id to all active clients
void all2_handle(){
  char buff[100];
  char buff2[20];

  printf("2ALL from: %d\n",mesg.id);
  sprintf(buff2,"Sender: %d\t", mesg.id);

  time_t curr_time;
  time(&curr_time);

  strftime(buff, 100, "%Y-%m-%d_%H-%M-%S ", localtime(&curr_time));

  strcat(buff,buff2);
  strcat(buff,mesg.mesg_text);
  strcpy(mesg.mesg_text,buff);

  for(int i = 0 ; i < CLIENT_NO ; i++)
    if(client_list[i]!= (mqd_t)(-1))
      send_message_to_client(i, ALL2_PRIOR);

}

// the same funcions as all2_handle but message is send only for friends of given client
void friends2_handle(){
  char buff[100];
  char buff2[20];

  printf("2FRIENDS from: %d\n",mesg.id);


  time_t curr_time;
  time(&curr_time);

  strftime(buff, 100, "%Y-%m-%d_%H-%M-%S ", localtime(&curr_time));
  sprintf(buff2,"Sender: %d\t", mesg.id);

  strcat(buff,buff2);
  strcat(buff,mesg.mesg_text);
  strcpy(mesg.mesg_text,buff);

  for(int i = 0 ; i < CLIENT_NO ; i++)
    if(friends[mesg.id][i])
      send_message_to_client(i, FRIENDS2_PRIOR);
}

// function for sending message to client with given id
void one2_handle(){
  char buff[100];
  char buff2[30];
  time_t curr_time;
  time(&curr_time);

  printf("2ONE from: %d\n",mesg.id);
  strftime(buff, 100, "%Y-%m-%d_%H-%M-%S ", localtime(&curr_time));

  sprintf(buff2,"Sender: %d\t", mesg.id);
  char* ptr = mesg.mesg_text;
  char* client = strtok_r(ptr," ",&ptr); // set delimeter

  int client_id = strtol(client,NULL,10); // read client id
  strcat(buff,buff2);
  strcat(buff,ptr);
  strcpy(mesg.mesg_text,buff);

  send_message_to_client(client_id, ONE2_PRIOR);
}


void add_handle(){
  printf("ADD from: %d\n",mesg.id);
  friend_list_handle(mesg.id,mesg.mesg_text, 'a');
  print_friend_list(mesg.id);
}

void del_handle(){
  printf("DELETE from: %d\n",mesg.id);
  friend_list_handle(mesg.id,mesg.mesg_text, 'd');
  print_friend_list(mesg.id);
}

void stop_handle(){
  printf("STOP from: %d\n",mesg.id);

  // delete client friends
  delete_client_friends_list(mesg.id);

  // close client queue
  mq_close(client_list[mesg.id]);

  client_list[mesg.id]=-1;
}

void echo_handle(){
  char buff[100];
  time_t curr_time;
  time(&curr_time);

  printf("ECHO from: %d\n",mesg.id);
  strftime(buff, 100, "%Y-%m-%d %H-%M-%S ", localtime(&curr_time));

  strcat(buff, mesg.mesg_text); // add message text to buff
  strcpy(mesg.mesg_text, buff); // copy buff to message text
  send_message_to_client(mesg.id, ECHO_PRIOR); // send message
}




void friends_handle(){
  delete_client_friends_list(mesg.id); // clear existing list

  if(mesg.mesg_text[0] != '\0'){ // if command friends had arguments
    friend_list_handle(mesg.id, mesg.mesg_text, 'a');
  }
  print_friend_list(mesg.id);
}


void check_client_list(){
  for( int i = 0 ; i < CLIENT_NO ; ++i)
    printf("%d %d\n",i,client_list[i] );
}

/**************************
INITIALIZE AND EXIT FUNCTIONS
**************************/

void set_up_client_list(){
  for( int i = 0 ; i < CLIENT_NO ; i++){
    client_list[i]= (mqd_t)(-1); // cast -1 to sqd_t

    for(int j = 0 ; j < CLIENT_NO ; j++)
      friends[i][j]=0;
  }
}


void set_up_server_queue_id(){
  mq_unlink(SERVER_NAME); // remove specified queue

  // set queue attributes
  struct mq_attr attr;
  attr.mq_flags = O_NONBLOCK; // Flags: 0 or O_NONBLOCK
  attr.mq_maxmsg = 10; // max messages in queue
  attr.mq_msgsize = sizeof(mesg); // max message size
  attr.mq_curmsgs = 0; // # of messagess currently in queue

//mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr);
// O_RDONLY - Open the queue to receive messages only.
// O_CREAT - Create the message queue if it does not exist.
// 0777 - permisions
  if(( queue_id = mq_open(SERVER_NAME, O_RDONLY | O_CREAT, 0777, &attr )) == -1 ){
    printf("ERROR:%s\n",strerror(errno));
    exit(1);
  }
}

void sigint_handler(){
  exit(0);
}
//////////////////////////


int main(int argc, char** argv){

  srand(time(NULL));

  signal(SIGINT, sigint_handler);
  atexit(exit_function);  // normal process termination

  set_up_client_list(); // initialize array with clients and friends
  set_up_server_queue_id(); // create queue

  while(1){
    if(mq_receive(queue_id,(char *)&mesg,sizeof(mesg),NULL) != -1){
      switch (mesg.type) {
        case INIT:
          init_handle();
          break;

        case LIST:
          list_handle();
          break;

        case ECHO:
          echo_handle();
          break;

        case ALL2:
          all2_handle();
          break;

        case FRIENDS2:
          friends2_handle();
          break;

        case ONE2:
          one2_handle();
          break;

        case FRIENDS:
          friends_handle();
          break;

        case STOP:
          stop_handle();
          break;

        case ADD:
          add_handle();
          break;

        case DEL:
          del_handle();
          break;

        default:
          break;
        }
      }
    }

  return 0;
}
