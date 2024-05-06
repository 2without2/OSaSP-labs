#include <stdio.h>
#include <semaphore.h>
#include "buffer.h"

extern pthread_mutex_t* mutex;
extern sem_t* waitForFreeSpace;
extern sem_t* waitForAnyItem;

short hash(const msg_t* msg) {
    short hash = 123;

    for(size_t i = 0; i < sizeof(msg_t); i++) {
        hash = ((hash << 5) + hash) + ((short*) msg)[i];
    }
    return hash;
}

int put(buff_t* buff, msg_t* msg) {
    if(buff -> size == buff->maxsize) {
        return -1;
    }

    buff -> messages[buff -> tail] = *msg;
    ++buff -> tail;
    ++buff -> size;
    ++buff -> put_count;

    if(buff -> tail == buff->maxsize) {
        buff -> tail = 0;
    }

    return 0;
}

int pop(buff_t* buff, msg_t* msg) {
    if(buff -> size == 0) {
        return -1;
    }

    *msg = buff -> messages[buff -> head];
    ++buff -> head;
    --buff -> size;
    ++buff -> pop_count;

    if(buff -> head == buff->maxsize) {
        buff -> head = 0;
    }

    return 0;
}

void init(buff_t* buff, int maxsize) {
    buff ->maxsize = maxsize;
}

void increaseBuffer(buff_t* buff){
    pthread_mutex_lock(mutex);
    sem_post(waitForFreeSpace);
    fprintf(stdout, "Increase waitForFreeSpace\n");
    int value;
    sem_getvalue(waitForFreeSpace, &value);
    fprintf(stdout, "waitForFreeSpace = %d\n", value);
    ++buff -> maxsize;
    pthread_mutex_unlock(mutex);
}

void decreaseBuffer(buff_t* buff){
    pthread_mutex_lock(mutex);
    int value;
    sem_getvalue(waitForFreeSpace, &value);
    if (buff->maxsize <= 0) {
        fprintf(stderr, "Main_%lu: Can't decrease buffer size\n", pthread_self());
        fprintf(stdout, "Main_%lu: Buffer size = %zu\n", pthread_self(), buff->maxsize);
    }
    else if(value == 0) { //уменьшение приведет к потере информации, то есть значение семафора равно нулю, всё занято
        fprintf(stderr, "Main_%lu: Can't decrease buffer size\n", pthread_self());
        fprintf(stdout, "waitForFreeSpace = 0\n");
    }
    else {
        fprintf(stdout, "Decrease waitForFreeSpace\n");
        sem_wait(waitForFreeSpace);
        sem_getvalue(waitForFreeSpace, &value);
        fprintf(stdout, "waitForFreeSpace = %d\n", value);
        --buff->maxsize;
    }
    pthread_mutex_unlock(mutex);
}