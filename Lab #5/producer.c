#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "buffer.h"

extern buff_t* msgs;
extern pthread_t producers[];
extern size_t prodCount;

extern sem_t* waitForFreeSpace;
extern sem_t* waitForAnyItem;
extern  pthread_mutex_t* mutex;

void createMessage(msg_t* msg) {
    int size = rand() % 257;
    short type = rand() % 0x8;
    for (int i = 0; i < size; i++) {
        msg -> data[i] = rand() % 256;
    }
    if (size == 256) {
        msg -> size = 0;
    }
    else {
        msg -> size = size;
    }
    msg -> type = type;
    msg -> hash = 0;
    msg -> hash = hash(msg);
}

void* producerHandler() {

    srand(pthread_self()) ;
    msg_t* msg = (msg_t*) malloc(sizeof(msg_t));

    while(1) {
        createMessage(msg);
        sem_wait(waitForFreeSpace);
        pthread_mutex_lock(mutex);

        put(msgs, msg);

        pthread_mutex_unlock(mutex);
        sem_post(waitForAnyItem);

        fprintf(stdout, "\nProducer_%lu:\nType: %d\nSize: %d\nPut_count msg: %zu\n\n", pthread_self(), msg -> type, msg -> size, msgs->put_count);

        sleep(5);
    }
}

void createProducer() {
    if (prodCount >= PRODUCERS) {
        fprintf(stderr, "Main_%lu: Producers limit exceeded (prodCount >= 128)\n", pthread_self());
        return;
    }
    int result = pthread_create(&producers[prodCount], NULL, producerHandler, NULL);
    if(result != 0){
        fprintf(stderr, "Main_%lu: Can't create \"Producer\" (ERROR: pthread_create())\n", pthread_self());
        return;
    } else {
        fprintf(stdout, "Main_%lu: Created \"Producer\" with pthread_ID: %lu\n", pthread_self(), producers[prodCount]);
        prodCount++;
        return;
    }
}

void removeProducer() {
    if (prodCount <= 0) {
        fprintf(stderr, "Main_%lu: Can't remove Producer (prodCount <= 0)\n", pthread_self());
        return;
    }
    prodCount--;
    pthread_cancel(producers[prodCount]);
    pthread_join(producers[prodCount], NULL);
    fprintf(stdout, "Main_%lu: Removing \"Producer\" with pthread_ID: %lu\n", pthread_self(), producers[prodCount]);
}