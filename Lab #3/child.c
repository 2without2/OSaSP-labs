#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <stdbool.h>
#include <string.h>

#define MAX_ITERATION_NUMBER 100

int ITERATION = 0;
bool flagPrintStatistics = false;

struct Data {
    int var1;
    int var2;
} data;

struct CountOFPairs {
    int count_00;
    int count_01;
    int count_10;
    int count_11;
} counter = {0, 0, 0, 0};

void atexit_handler();
void timer_handler(int sig);
void USER1_handler(int sig);
void USER2_handler(int sig);
void segment_handler(int sig);
void resetCounter();
void initSignalHandlers();
void setTimer();

int main(int argc, char** argv) {

    atexit(atexit_handler);
    initSignalHandlers();
    setTimer();

    while (1) {
        data.var1 = 1;
        data.var2 = 1;
        data.var1 = 0;
        data.var2 = 0;
        if (ITERATION == MAX_ITERATION_NUMBER) {

            if(flagPrintStatistics == true) {

                char buf[256];
                char *st = (char *) &buf;
                size_t size = sprintf(buf,
                                      "\nCHILD process:\n  My name: %s\n  My PID: %d\n  My PARENT PID: %d\n  Statistics:\n   00 - %d\n   01 - %d\n   10 - %d\n   11 - %d\n",
                                      argv[0], getpid(), getppid(), counter.count_00, counter.count_01, counter.count_10, counter.count_11);
                for(size_t i = 0; i < size; i++)
                    fputc(*(st + i), stdout);
            }
            ITERATION = 0;
            resetCounter();
        }
    }

}

void atexit_handler() {
    printf("Success");
}

void timer_handler(int sig) {
    ITERATION++;
    if (data.var1 == 1) {
        if (data.var2 == 1)
            counter.count_11++;
        else counter.count_10++;
    } else if (data.var1 == 0) {
        if (data.var2 == 1)
            counter.count_01++;
        else counter.count_00++;
    } else exit(EXIT_FAILURE);
}

//обработчик для SIGUSR1
void USER1_handler(int sig) {
    printf("CHILD - FALSE - %d\n", getpid());
    flagPrintStatistics = false;
}

//обработчик для SIGUSR2
void USER2_handler(int sig) {
    printf("CHILD - TRUE - %d\n", getpid());
    flagPrintStatistics = true;
}

void segment_handler(int sig) {
    printf("MEMORY error\n");
    exit(EXIT_FAILURE);
}

void resetCounter() {
    counter.count_00 = 0;
    counter.count_01 = 0;
    counter.count_10 = 0;
    counter.count_11 = 0;
}

void initSignalHandlers() {
    struct sigaction sa_user1, sa_user2, sa_timer, sa_memory;

    memset(&sa_timer, 0, sizeof(sa_timer));
    sa_timer.sa_handler = timer_handler;
    if (sigaction(SIGALRM, &sa_timer, NULL) == -1) {
        fprintf(stderr, "Error SIGALRM\n");
        exit(EXIT_FAILURE);
    }

    //sa_user1.sa_flags = SA_RESTART;
    sa_user1.sa_handler = USER1_handler;
    if (sigaction(SIGUSR1, &sa_user1, NULL) == -1) {
        fprintf(stderr, "Error SIGUSR1\n");
        exit(EXIT_FAILURE);
    }

    //sa_user2.sa_flags = SA_RESTART;
    sa_user2.sa_handler = USER2_handler;
    if (sigaction(SIGUSR2, &sa_user2, NULL) == -1) {
        fprintf(stderr, "Error SIGUSR1\n");
        exit(EXIT_FAILURE);
    }

    //sa_memory.sa_flags = SA_RESTART;
    sa_memory.sa_handler = segment_handler;
    if(sigaction(SIGSEGV, &sa_memory, NULL) == -1){
        perror("Error SIGSEGV\n");
        exit(EXIT_FAILURE);
    }
}

void setTimer() {
    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 10000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 10000;
    setitimer(ITIMER_REAL, &timer, NULL);
}