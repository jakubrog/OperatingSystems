#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>

#include "common.h"

void handle(struct message *msg);

void echo(struct message *msg);
void list(struct message *msg);
void to_friends(struct message *msg);
void all(struct message *msg);
void friends(struct message *msg);
void one(struct message *msg);
void stop(struct message *msg);
int find_client(pid_t sender_pid);
int create_message(struct message *msg);
char *getTime();

int clients[MAX_CLIENTS][2];
int client_count = 0;
int queue_id;
int active = 1;

/// TODO: LOGIN TAK PRZEROBIC ZEBY DZIALAL JAKO LADOWANIE ZNJOMYCH, DOROBIC WSZYSTKIE FUNKCJE. DODAC OBSLUGE SIGINT



int main(){
  struct msqid_ds current_state;

  char* path = getenv("HOME"); //get home path in path
  if(!path)
    return 1;

  key_t publicKey = ftok(path, PROJECT_ID); // generate key value
  if(!publicKey)
    return 2;

  queue_id = msgget(publicKey,IPC_CREAT | IPC_EXCL | 0666); //what are these flags
  if(queue_id != 0)
    return 3;

  message msg;
  while(1){
      if(active == 0){
        //msgctl - function provide message control operations as specified by IPC_STAT - Place the current value of each member of the msqid_ds data structure associated with msqid into the structure pointed to by buf.
          if(msgctl(queue_id, IPC_STAT, &current_state) != 0)
              return 4;
          if (current_state.msg_qnum == 0) // if queue is empty
              break;


//int msgrcv(int msgqid, struct msgbuf *msgp, int msgsz,long type, int msgflg) -6 means the first message of the lowest type that is less than or equal to the absolute value of msgtyp shall be received.
          if (msgrcv(queue_id, &msg, MSG_SIZE, -6, 0) < 0)
              return 5;
          handle(&msg);
      }
  }
  return 0;
}


void handle(struct message *msg){
  if (msg == NULL)
      return;
  switch(msg->mtype){
      case LOGIN:

          break;
      case ECHO:

          break;
      case LIST:

          break;
      case TOFRIENDS:

          break;
      case ALL:

          break;
      case FRIENDS:

          break;
      case ONE:

          break;
      case STOP:

          break;
      default:
          break;
      }
}

void login(message *msg){
    key_t client;
    if(sscanf(msg->message_text, "%d", &client)) //read formated data from string
        exit(6);

    int client_queue_id = msgget(client, 0);
    int client_pid = msg->sender_pid;

    msg->mtype = 0;
    msg->sender_pid = getpid();

    if(client_count > MAX_CLIENTS - 1)
        exit(7);

    else{
      clients[client_count][0] = client_pid;
      clients[client_count++][1] = client_queue_id;
      sprintf(msg->message_text, "%d", client_count - 1);
    }

    if(msgsnd(client_queue_id, msg, MSG_SIZE, 0) == -1)
        exit(8);
}

void echo(message *msg){
    int client_id =  create_message(msg);
    if(client_id == -1)
        exit(9);
    char *reply = strcat(msg->message_text, getTime());
    strcpy(msg->message_text, reply);

    if(msgsnd(client_id, msg, MSG_SIZE, 0) == -1)
        exit(1);
}

int create_message(struct message *msg) {
    int client_id = find_client(msg->sender_pid);
    if (client_id == -1){
        printf("server: client not found\n");
        return -1;
    }

    msg->mtype = msg->sender_pid;
    msg->sender_pid = getpid();

    return client_id;
}

int find_client(pid_t sender_pid){
  for (int i=0; i < MAX_CLIENTS; ++i) {
      if(clients[i][0] == sender_pid)
          return clients[i][1];
  }
  return -1;
}

char *getTime(){
    time_t current_time;
    struct tm * time_info;
    char *timeString = malloc(sizeof(char) * 30);  // space for "HH:MM:SS\0"

    time(&current_time);
    time_info = localtime(&current_time);

    strftime(timeString, 20, "%b %d %H:%M", time_info);
    return timeString;
}
