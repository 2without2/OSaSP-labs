#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

//  export CHILD_PATH=/home/user/CLionProjects/Lab_2/child
//  ./parent /home/user/CLionProjects/Lab_2/file.txt

int compare(const void *string1, const void *string2);
void changeNameChildProcess(char* child_XX);
char* findChildPath(char** array);
void Fork(char* childPath, char** argv, char** env);
void printSortedEnvParam(char ** envp);

int main(int argc, char** argv, char** envp) {
    printSortedEnvParam(envp);                                      //output sorted environment values
    //argv[0] - name of process, argv[1] - path to file with var
    char** newCommandArgv;
    if(!(newCommandArgv = (char**) calloc(2, sizeof(char*)))){
        fprintf(stderr, "Memory problem");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < 2; i++){
        if(!(*(newCommandArgv + i) = (char*) calloc(100, sizeof(char)))){
            fprintf(stderr, "Memory problem\n");
            exit(EXIT_FAILURE);
        }
    }
    strcpy(*(newCommandArgv + 1), argv[1]); //get ENVP_PATH

    //Цикл обработки нажатий клавиатуры
    int flag = 1;
    while(1) {
        if(flag == 1) {
            printf("Please, enter symbol...\n");
            flag = 0;
        }
        char ch;
        ch = (char)getc(stdin);
        rewind(stdin);

        switch (ch) {
            case '+':
                changeNameChildProcess(*(newCommandArgv + 0));
                Fork(getenv("CHILD_PATH"), newCommandArgv, envp);
                break;
            case '*':
                changeNameChildProcess(*(newCommandArgv + 0));
                Fork(findChildPath(envp), newCommandArgv, envp);
                break;
            case '&': {
                extern char** environ;
                changeNameChildProcess(*(newCommandArgv + 0));
                Fork(findChildPath(environ), newCommandArgv, envp);
                break;
            }
            case '\n':
                flag = 1;
                break;
            case 'q': {
                printf("Parent process has finished\n");
                return 1;
            }
            default:
                printf("Symbol Error\n");
        }
    }
}

void printSortedEnvParam(char ** envp) {
    char** temp = envp;
    size_t count = 0;

    while(*temp++)
        count++;

    char** envp_copy = NULL;
    if((envp_copy = (char**)calloc(count, sizeof(char*))) == NULL){
        perror("Memory error");
        return;
    }

    for(size_t i = 0; i < count; i++) {
        size_t lenght = strlen(*(envp + i));
        *(envp_copy + i) = (char*) calloc(lenght + 1, sizeof(char));
        strcpy(*(envp_copy + i), *(envp + i));
        *(*(envp_copy + i) + lenght) = '\0';
    }
    //Сортировка переменных среды в родительском процессе
    qsort(envp_copy, count, sizeof(char*), (int(*) (const void*, const void*))compare);

    for(size_t i = 0; i < count; i++)
        printf("%s\n", *(envp_copy + i));

    for(size_t i = 0; i < count; i++)
        free(*(envp_copy + i));
    free(envp_copy);
}

int compare(const void *string1, const void *string2) {
    return strcmp(*(char **)string1, *(char **)string2);
}

void changeNameChildProcess(char* child_XX) {
    static int processNumber = 0;
    char* XX = NULL;
    if(!(XX = (char*) calloc(3, sizeof(char)))){
        perror("Memory error\n");
        exit(EXIT_FAILURE);
    }

    if(processNumber < 10){
        char ch = '0' + processNumber;
        *XX = '0';
        *(XX + 1) = ch;
        *(XX + 2) = '\0';
    }
    else if(processNumber >= 10){
        char firstPart = '0' + (int)(processNumber / 10);
        char secondPart = '0' + (processNumber % 10);
        *XX = firstPart;
        *(XX + 1) = secondPart;
        *(XX + 2) = '\0';
    }
    else if(processNumber > 99){
        processNumber = 0;
        free(XX);
        return;
    }

    strcpy(child_XX, "child_");
    strcat(child_XX, XX);

    processNumber++;
    free(XX);
}

void Fork(char* childPath, char** argv, char** envp) {
    int child_status;
    pid_t pid = fork();

    if(pid == -1) fprintf(stderr, "Process Error!\n");
    else if(pid == 0) execve(childPath, argv, envp);

    sleep(2);
    wait(&child_status);
    printf("CHILD process have ended with %d exit status\n\n", child_status);
}

char* findChildPath(char** array){
    char* childPathFromEnv = NULL, *temp = NULL;
    if(!(childPathFromEnv = (char*) calloc(256, sizeof(char)))){
        printf("Memory error\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; *(array + i) != NULL; i++ ) {
        temp = *(array + i);

        if(strstr(temp, "CHILD_PATH") != NULL){
            strcpy(childPathFromEnv, temp + 11);
            break;
        }
    }
    return childPathFromEnv;
}
