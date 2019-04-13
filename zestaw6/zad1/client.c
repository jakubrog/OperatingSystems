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

void register_client(key_t privateKey);
void request_mirror(Message *msg);
void request_calc(Message *msg);
void request_time(Message *msg);
void request_end(Message *msg);
int create_queue(char*, int);
void close_queue();
void int_handler(int);

int queue_id;
int privateID;

int main(){
  struct msqid_ds current_state;

  char* path = getenv("HOME"); //get home path in path
  if(!path)
    return 1;

  int queue_id = msgget(ftok(path, PROJECT_ID), 0); //get queue id

  key_t privateKey = ftok(path, getpid()); // generate privateKey

  if(!privateKey)
    return 2;

  privateID = msgget(privateKey, IPC_CREAT | IPC_EXCL | 0666);

 //what are these flags
  if(queue_id != 0)
    return 3;


}
void register(key_t privateKey){

    
}

int create_queue(char *path, int ID) {
    int key = ftok(path, ID);
    if(key == -1) FAILURE_EXIT("Generation of key failed");

    int QueueID = msgget(key, 0);
    if (QueueID == -1) FAILURE_EXIT("Opening queue failed");

    return QueueID;
}
