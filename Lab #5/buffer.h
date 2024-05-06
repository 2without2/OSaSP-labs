#include <sys/types.h>
#include <pthread.h>


#ifndef LAB_5_BUFFER_H
#define LAB_5_BUFFER_H
#define PRODUCERS 128
#define CONSUMERS 128

typedef struct {
    char type;
    short hash;
    int size;
    char data[256];
} msg_t;

typedef struct {
    size_t put_count;
    size_t pop_count;

    size_t head;
    size_t tail;
    size_t size;

    size_t maxsize;
        
    msg_t messages[2048];
} buff_t;

short hash(const msg_t* msg);

int put(buff_t* buff, msg_t* msg);
int pop(buff_t* buff, msg_t* msg);

void init(buff_t* buff, int maxsize);
void increaseBuffer(buff_t* buff);
void decreaseBuffer(buff_t* buff);

#endif //LAB_5_BUFFER_H
