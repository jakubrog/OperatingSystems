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

#include "headers.h"

#define CLIENT_NO 30

// one static message for whole program, easier to use in funcitions and there is no need for more
static struct mesg_buffer mesg;
static int queue_id = -1;

static int friends[CLIENT_NO][CLIENT_NO]; // every firend can have a list with friends

// arraty with clients, where every client has its own id. Under this id in array there is queue_id
static int client_list[CLIENT_NO];


/**************************
MESSAGE HANDLERS
**************************/

// normal process termination
void exit_function(){
  struct mesg_buffer exit_message;
  exit_message.priority = STOP_PRIOR;
  exit_message.type = STOP;

  for(int i = 0 ; i < CLIENT_NO ; ++i){
    if(client_list[i]!= -1){
      msgsnd(client_list[i],&exit_message,mesg_size(),0);
    }
  }

    if(queue_id != -1) // if queue exists
      msgctl(queue_id, IPC_RMID, NULL); // controll queue with queue_id, IPC_RMID - immidiately remove the message queue
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
void send_message_to_client(int client_id){
  if(client_id < CLIENT_NO && client_list[client_id] != -1){
    printf("Message sent to: %d \n\n",client_list[client_id]);
    msgsnd(client_list[client_id], &mesg, mesg_size(), 0);
  }
}

// simple function to add client
int add_client(int queue_id){
  for( int i = 0 ; i < CLIENT_NO ; ++i)
    if(client_list[i] == -1){
      client_list[i]=queue_id;
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

  if( (client_id = add_client(mesg.id)) == -1){
    printf("Cannot initialize client\n");
  }

  printf("Client no %d has been initialized\n", client_id);
  mesg.type = INIT;
  mesg.priority= INIT_PRIOR;
  mesg.id = client_id;
  send_message_to_client(client_id);
}

void list_handle(){
  int client_id = mesg.id;
  printf("LIST from: %d\n",mesg.id);
  list_clients();
  mesg.type = LIST;
  mesg.priority = LIST_PRIOR;
  send_message_to_client(client_id);
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
    if(client_list[i]!= -1)
      send_message_to_client(i);

}

// the same funcions as all2_handle but message is send only for friends of given client
void friends2_handle(){
  char buff[100];
  char buff2[30];

  printf("2FRIENDS from: %d\n",mesg.id);


  time_t curr_time;
  time(&curr_time);

  strftime(buff, 100, "%Y-%m-%d_%H-%M-%S ", localtime(&curr_time));

  strcat(buff,buff2);
  strcat(buff,mesg.mesg_text);
  strcpy(mesg.mesg_text,buff);
  for(int i = 0 ; i < CLIENT_NO ; i++)
    if(friends[mesg.id][i])
      send_message_to_client(i);
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

  send_message_to_client(client_id);
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
  delete_client_friends_list(mesg.id);
  client_list[mesg.id]=-1;
}

void echo_handle(){
  char buff[100];
  time_t curr_time;
  time(&curr_time);

  printf("ECHO from: %d\n",mesg.id);
  strftime(buff, 100, "%Y-%m-%d_%H-%M-%S ", localtime(&curr_time));

  strcat(buff, mesg.mesg_text); // add message text to buff
  strcpy(mesg.mesg_text, buff); // copy buff to message text
  send_message_to_client(mesg.id); // send message
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
  for( int i = 0 ; i < CLIENT_NO ; ++i){
    client_list[i]= -1;

    for(int j = 0 ; j < CLIENT_NO ; ++j)
      friends[i][j]=0;
  }
}


void set_up_server_queue_id(){
  const char *homedir;

  // getuid to get the user id of the current user and then getpwuid to get the password entry (which includes the home directory) of that user:

  if ((homedir = getenv("HOME")) == NULL) {
      homedir = getpwuid(getuid())->pw_dir;
  }
  printf("%s\n", homedir);
  key_t key_own= ftok(homedir, SERVER_SEED);

  if(( queue_id = msgget(key_own, 0777 | IPC_CREAT )) == -1 ){ // get access or create message queu, IPC_CREAT to create queue if doesnt exist
    printf("Cannot create or open queue\n" );
    exit(-1);
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
    if(msgrcv(queue_id, &mesg, mesg_size() ,0,IPC_NOWAIT) != -1){
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
