#include <sys/mman.h>

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

#include "buffer.h"
#include "producer.h"
#include "consumer.h"

#include <pthread.h>

#define BUFFER_SIZE 5

buff_t *msgs;

pthread_t producers[PRODUCERS];
pthread_t consumers[CONSUMERS];
size_t prodCount = 0;
size_t consCount = 0;

sem_t* waitForFreeSpace;
sem_t* waitForAnyItem;
pthread_mutex_t* mutex;

void printStats() {
    pthread_mutex_lock(mutex);
    fprintf(stdout, "Main_%lu: Buffer stats: puts: %zu, pops: %zu, size: %zu\n",pthread_self(), msgs -> put_count, msgs -> pop_count, msgs -> size);
    fprintf(stdout, "Main_%lu: \"Producers\": count: %zu\n", pthread_self(), prodCount);
    for (size_t i = 0; i < prodCount; i++) {
        fprintf(stdout, "Main_%lu: Running \"Producer_%lu\"\n", pthread_self(), producers[i]);
    }
    fprintf(stdout, "Main_%lu: \"Consumers\": count: %zu\n", pthread_self(), consCount);
    for (size_t i = 0; i < consCount; i++) {
        fprintf(stdout, "Main_%lu: Running \"Consumer_%lu\"\n", pthread_self(), consumers[i]);
    }
    pthread_mutex_unlock(mutex);
    fprintf(stdout, "\n");
}

void removeOnExit() {
    if (prodCount == 0) {
        fprintf(stdout, "Main_%lu: No running \"Producers\" found\n", pthread_self());
    }

    sem_destroy(waitForFreeSpace);
    sem_destroy(waitForAnyItem);
    if(pthread_mutex_destroy(mutex)){
        fprintf(stderr, "Failed mutex init!\n");
        exit(1);
    }
    fprintf(stdout, "Main_%d: Deleting completed\n", getpid());
}

int main() {

    char* p = mmap(NULL, 2 * sizeof(sem_t) + sizeof(pthread_mutex_t) + sizeof(buff_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
                                                                                                                                                //Флаг MAP_ANONYMOUS указывает операционной системе, что отображение памяти должно быть анонимным и не должно быть связано с каким-либо файлом
    if (p == MAP_FAILED) {
        fprintf(stderr, "Main_%d: Can't map bytes", getpid());
        exit(1);
    }

    waitForFreeSpace = (sem_t*) (p + 0);
    waitForAnyItem = (sem_t*) (p + 1 * sizeof(sem_t));
    mutex = (pthread_mutex_t*) (p + 2 * sizeof(sem_t));
    msgs = (buff_t*) (p + 2 * sizeof(sem_t) + sizeof(pthread_mutex_t));

    if(pthread_mutex_init(mutex, NULL)){
        fprintf(stderr, "Main_%lu: Program failed at initializing mutex\n", pthread_self());
        exit(1);
    }
    if (sem_init(waitForFreeSpace, 1, BUFFER_SIZE) + sem_init(waitForAnyItem, 1, 0) != 0) {
        fprintf(stderr, "Main_%lu: Program failed at initializing semaphores\n", pthread_self());
        exit(1);
    }

    init(msgs, BUFFER_SIZE);

    char choice;
    fprintf(stdout, "Menu:\np - Create producer\nc - Create consumer\nw - Remove producer\ne - Remove consumer\n+ - Increase buffer\n- - Decrease buffer\nq - Quit\n\n");
    while (1) {
        choice = (char)getc(stdin);
        if (choice == 'p') {
            createProducer();
        }
        else if (choice == 'c') {
            createConsumer();
        }
        else if (choice == 'w') {
            removeProducer();
        }
        else if (choice == 'e') {
            removeConsumer();
        }
        else if (choice == 's') {
            printStats();
        }
        else if (choice == 'q') {
            removeOnExit();
            munmap(p, 2 * sizeof(sem_t) + sizeof(pthread_mutex_t) + sizeof(buff_t));
            fprintf(stdout, "Quitting...\n");
            break;
        }
        else if(choice == '+') {
            increaseBuffer(msgs);
        }
        else if(choice == '-'){
            decreaseBuffer(msgs);
        }
        else {
            fprintf(stdout, "\n\nUnknown option... Try these:\n[p] - Create producer\n[c] - Create consumer\n[w] - Remove producer\n[e] - Remove consumer\n[q] - Quit\n\n");
        }
        fprintf(stdout, "Option: %c\n", choice);
        getchar();
    }
    return 0;
}