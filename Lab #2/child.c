#include "stdio.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    printf("\nHello! I am CHILD process\n");
    printf("  My name: %s\n", argv[0]);
    printf("  My pid: %d\n", getpid());
    printf("  My ppid: %d\n", getppid());

    FILE* file = NULL;
    char* name = NULL;

    if(!(file = fopen(argv[1], "r"))) {
        fprintf(stderr, "Error with file open\n");
        exit(EXIT_FAILURE);
    }

    if(!(name = (char*) calloc(100, sizeof(char)))){
        fprintf(stderr, "Memory error");
        exit(EXIT_FAILURE);
    }

    int index = 1;
    while(1){
        fscanf(file, "%s", name);
        if(feof(file)) break;
        fprintf(stdout, "%d) %s = %s\n", index++, name, getenv(name));
    }

    fclose(file);
    sleep(15);
    return 0;
}