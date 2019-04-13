#ifndef COMMON_H
#define COMMON_H

#define MAX_CLIENTS 10
#define PROJECT_ID 0x99
#define MAX_CONT_SIZE 4096

typedef enum message_type{
    ECHO = 1, LIST = 2, TOFRIENDS = 3, ALL = 4, FRIENDS = 5, ONE = 6, STOP = 7,
    LOGIN = 8, LOGGED = 9
} message_type;

typedef struct message{
    long mtype;
    pid_t sender_pid;
    char message_text[MAX_CONT_SIZE];
} message;

// msgsz does not contain mtype
const size_t MSG_SIZE = sizeof(message) - sizeof(long);

#endif
