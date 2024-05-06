#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 10
#define CHILD_PATH "/home/user/CLionProjects/Lab_3/child"
#define MAX_NUMBER_OF_PROCESS 20
int NUMBER_OF_PROCESS = 0;
int ARRAY_OF_CHILD_PROCESS[MAX_NUMBER_OF_PROCESS] = {0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0};

struct StringElements{
    char ch;
    int number;
};

struct StringElements stringParser(char* s);
int Atoi(char* s);
void showContextMenu();
void showContextMenu_RU();
void initSignalHandlers();
void USER1_handler(int sig);
void alarm_handler();
int deleteProcess(pid_t);
int displayProcessList();
int addProcess();
void muteProcess(pid_t);
void unmuteProcess(pid_t);

int main(){
    char* answer = NULL;
    if(!(answer = calloc(MAX_LINE, sizeof(char)))) {
        fprintf(stderr, "Memory error\n");
        exit(EXIT_FAILURE);
    }

    initSignalHandlers();
    showContextMenu_RU();

    while(1) {
        fgets(answer, MAX_LINE, stdin);
        struct StringElements SE = stringParser(answer);

        switch (SE.ch) {
            case '+':
                if(!addProcess())
                    fprintf(stderr, "The maximum number of processes has already been created\n");
                break;
            case '-':
                if(!deleteProcess(ARRAY_OF_CHILD_PROCESS[NUMBER_OF_PROCESS - 1]))
                    fprintf(stderr, "Processes do not exist\n");
                break;
            case 'l':
                if(!displayProcessList())
                    printf("List is empty\n");
                break;
            case 'k':
                for(int i = NUMBER_OF_PROCESS - 1; i >= 0; i--)
                    if(!deleteProcess(ARRAY_OF_CHILD_PROCESS[i])) {
                        fprintf(stderr, "Processes do not exist\n");
                        break;
                    }
                break;
            case 's': // - ЗАПРЕТ - SIGUSR1
                if(SE.number == 0){ //всем процессам запрещает выводить стату
                    if(NUMBER_OF_PROCESS > 0){
                        for(int i = 0; i < NUMBER_OF_PROCESS; i++)
                            muteProcess(ARRAY_OF_CHILD_PROCESS[i]);
                    }
                }
                else{ // процессу с id SE.number
                    int i;
                    for(i = 0; i < NUMBER_OF_PROCESS; i++)
                        if(SE.number == ARRAY_OF_CHILD_PROCESS[i]){
                            muteProcess(ARRAY_OF_CHILD_PROCESS[i]);
                            break;
                        }
                    if(i == NUMBER_OF_PROCESS)
                        printf("Process  with  pid = %d does not exist\n", SE.number);
                }
                break;
            case 'g': // - РАЗРЕШАЕТ - SIGUSR2
                if(SE.number == 0){ //всем процессам разрешает выводить стату
                    if(NUMBER_OF_PROCESS > 0){
                        for(int i = 0; i < NUMBER_OF_PROCESS; i++)
                            unmuteProcess(ARRAY_OF_CHILD_PROCESS[i]);
                    }
                }
                else{ // процессу с id SE.number
                    int i;
                    for(i = 0; i < NUMBER_OF_PROCESS; i++)
                        if(SE.number == ARRAY_OF_CHILD_PROCESS[i]){
                            unmuteProcess(ARRAY_OF_CHILD_PROCESS[i]);
                            break;
                        }
                    if(i == NUMBER_OF_PROCESS)
                        printf("Process  with  pid = %d does not exist\n", SE.number);
                }
                break;
            case 'p':
                if(SE.number != 0 && NUMBER_OF_PROCESS > 0){
                    for (size_t i = 0; i < NUMBER_OF_PROCESS; ++i)
                        muteProcess(ARRAY_OF_CHILD_PROCESS[i]);

                    int j;
                    for(j = 0; j < NUMBER_OF_PROCESS; j++)
                        if(SE.number == ARRAY_OF_CHILD_PROCESS[j]){
                            unmuteProcess(ARRAY_OF_CHILD_PROCESS[j]);
                            break;
                        }
                    if(j == NUMBER_OF_PROCESS)
                        printf("Process  with  pid = %d does not exist\n", SE.number);

                    alarm(5);
                    pause();

                    for(size_t i = 0; i < NUMBER_OF_PROCESS;i++ ){
                        if(SE.number != ARRAY_OF_CHILD_PROCESS[i])
                            unmuteProcess(ARRAY_OF_CHILD_PROCESS[i]);
                    }
                }
                break;
            case 'c':
                system("clear");
                break;
            case 'q':
                for(int i = NUMBER_OF_PROCESS - 1; i >= 0; i--)
                    if(!deleteProcess(ARRAY_OF_CHILD_PROCESS[i])) {
                        fprintf(stderr, "Processes do not exist\n");
                        break;
                    }
                free(answer);
                return 0;
        }
    }
}

struct StringElements stringParser(char* str) {
    struct StringElements temp = {'a', 0};
    switch(str[0]){
        case '+': temp.ch = '+'; break;
        case '-': temp.ch = '-'; break;
        case 'l': temp.ch = 'l'; break;
        case 'k': temp.ch = 'k'; break;
        case 's': temp.ch = 's'; break;
        case 'g': temp.ch = 'g'; break;
        case 'p': temp.ch = 'p'; break;
        case 'c': temp.ch = 'c'; break;
        case 'q': temp.ch = 'q'; break;
        default:
            temp.number = -1;
            return temp;
    }
    if(strlen(str) > 1 && (temp.ch == 's' || temp.ch == 'g' || temp.ch == 'p')) {
        if((temp.number = Atoi(str)) == -1){
            printf("Error Command Arguments\n");
            exit(EXIT_FAILURE);
        }
    }
    return temp;
}

int Atoi(char* s){
    double number = 0;
    int factor = 1;
    size_t i = strlen(s) - 2;
    while(i > 0){
        if(s[i] >= '0' && s[i] <= '9')
            number += (s[i] - '0') * factor;
        else return -1;
        factor *= 10;
        i--;
    }
    return (int)number;
}

void showContextMenu_RU() {
    printf("\nc - очистить экран\n");
    printf("+ - добавить дочерний процесс\n");
    printf("- - удалить последний дочерний процесс\n");
    printf("l - список родительских и дочерних процессов\n");
    printf("k - удалить все дочерние процессы\n");
    printf("s - запретить дочерним процессам отображать статистику\n");
    printf("g - разрешить дочерним процессам отображать статистику \n");
    printf("s<num> - запрещает дочернему процессу c PID=num выводить статистику\n");
    printf("g<num> - разрешает дочернему процессу c PID=num выводить статистику\n");
    printf("p<num> - запрещает, потом разрешает\n");
    printf("q - удалить все дочерний процессы и завершить программу\n\n");
}

void initSignalHandlers() {

    struct sigaction sa_alarm;

    sa_alarm.sa_flags = SA_RESTART;
    sa_alarm.sa_handler = alarm_handler;
    if(sigaction(SIGALRM, &sa_alarm, NULL) == -1) {
        perror("Signal error\n");
        exit(EXIT_FAILURE);
    }
}

void alarm_handler(int sig){
    printf("Time is over\n");
}

int deleteProcess(pid_t pid){
    if(NUMBER_OF_PROCESS > 0) {
        kill(pid, SIGTERM);
        printf("You delete CHILD process. Him pid = %d\n", pid);
        ARRAY_OF_CHILD_PROCESS[NUMBER_OF_PROCESS - 1] = 0;
        NUMBER_OF_PROCESS--;
        return 1;
    }
    else return 0;

}

int displayProcessList(){
    printf("PARENT process: %d\n", getpid());
    printf("List of CHILD process\n");
    if(NUMBER_OF_PROCESS > 0){
        for(int i = 0; i < NUMBER_OF_PROCESS; i++)
            printf(" %d) PID = %d\n", i + 1, ARRAY_OF_CHILD_PROCESS[i]);
        return 1;
    }
    else return 0;
}

//SIGCONT SIGSTOP

int addProcess() {
    if(NUMBER_OF_PROCESS <= MAX_NUMBER_OF_PROCESS){
        char* name = "Child process\0";
        pid_t pid = fork();
        if(pid == -1) {
            fprintf(stderr, "Process Error!\n");
            return 0;
        }
        else if(pid == 0) execl(CHILD_PATH, name, NULL);
        NUMBER_OF_PROCESS++;
        ARRAY_OF_CHILD_PROCESS[NUMBER_OF_PROCESS - 1] = pid;
        printf("You create CHILD process. Him pid = %d\n", pid);
        return 1;
    }
    else return 0;
}

void muteProcess(pid_t pid) {
    kill(pid, SIGUSR1);
    //kill(pid, SIGSTOP);Ы
}

void unmuteProcess(pid_t pid) {
    //kill(pid, SIGCONT);
    kill(pid, SIGUSR2);
}