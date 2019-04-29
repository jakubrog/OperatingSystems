#ifndef headers_h
#define headers_h

// priority of each message
#define STOP_PRIOR 1
#define LIST_PRIOR 2
#define INIT_PRIOR 4
#define ECHO_PRIOR 4
#define FRIENDS_PRIOR 3
#define ADD_PRIOR 4
#define DEL_PRIOR 4
#define ALL2_PRIOR 4
#define ONE2_PRIOR 4
#define FRIENDS2_PRIOR 4

#define PRIORITY -4

// every message is identified by type defined below
#define STOP 12
#define LIST 11
#define INIT 10
#define ECHO 9
#define FRIENDS 7
#define ADD 6
#define DEL 5
#define ALL2 4
#define ONE2 3
#define FRIENDS2 2


#define SERVER_SEED 10


#define MESSAGE_SIZE 100

struct mesg_buffer {
    long priority; // mtype
    char mesg_text[100];
    int id; // queue id
    int type; // message type
} mesg_buffer;

int mesg_size(){
  return (sizeof(mesg_buffer) - sizeof(long));
}

#endif
