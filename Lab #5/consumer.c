#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include "buffer.h"

extern buff_t* msgs;
extern pthread_t consumers[];
extern size_t consCount;

extern sem_t* waitForFreeSpace;
extern sem_t* waitForAnyItem;
extern pthread_mutex_t* mutex;

void* consumerHandler(){

    srand(pthread_self());
    msg_t* msg = (msg_t*) malloc(sizeof(msg_t));

    while(1) {
        sem_wait(waitForAnyItem);
        pthread_mutex_lock(mutex);

        pop(msgs, msg);

        pthread_mutex_unlock(mutex);
        sem_post(waitForFreeSpace);

        fprintf(stdout, "\nConsumer_%lu: \nPop_count msg: %zu\ndata: ", pthread_self(), msgs->pop_count);
        for (size_t i = 0; i < (size_t) msg -> size; i++) {
            fprintf(stdout, "%hhx ", msg -> data[i]);
        }
        fprintf(stdout, "\nType: %hhx", msg -> type);
        fprintf(stdout, "\nSize: %d", msg -> size);
        fprintf(stdout, "\n");

        sleep(5);
    }
}

void createConsumer() {
    if (consCount >= CONSUMERS) {
        fprintf(stderr, "Main_%lu: \"Consumers\" limit exceeded (consCount >= 128)\n", pthread_self());
        return;
    }
    int result = pthread_create(&consumers[consCount], NULL, consumerHandler, NULL);
    if (result != 0) {
        fprintf(stderr, "Main_%lu: Can't create \"Consumer\" (ERROR: pthread_create())\n", pthread_self());
        return;
    }
    if (consumers[consCount] != 0) {
        fprintf(stdout, "Main_%lu: Created \"Consumer\" with pthread_ID: %lu\n", pthread_self(), consumers[consCount]);
        consCount++;
        return;
    }
}

void removeConsumer() {
    if (consCount <= 0) {
        fprintf(stderr, "Main_%lu: Can't remove \"Consumer\" (consCount <= 0)\n", pthread_self());
        return;
    }

    consCount--;
    pthread_cancel(consumers[consCount]);
    pthread_join(consumers[consCount], NULL);
    fprintf(stdout, "Main_%lu: Removing \"Consumer\" with pthread_ID: %lu\n", pthread_self(), consumers[consCount]);
}