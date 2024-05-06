#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXSIZE 1024
//option flags
struct options{
    short symlink;
    short directory;
    short file;
    short sort;
};

char arrayOfPath[MAXSIZE][MAXSIZE];
static int countOfPath = 0;

void upgradePath(char* filePath, char* directionPath, char* part) {
    strcpy(filePath, directionPath);
    strcat(filePath, "/");
    strcat(filePath, part);
}

void printPath(char* path, struct dirent* dirp, struct options opt){
    if(opt.directory == 1 && dirp->d_type == DT_DIR) {
        printf("%s\n", path);
    }
    if(opt.file == 1 && dirp->d_type == DT_REG) {
        printf("%s\n", path);
    }
    if(opt.symlink == 1 && dirp->d_type == DT_LNK) {
        printf("%s\n", path);
    }
}

void savePathToArray(char* path, struct dirent* dirp, struct options opt){
    if(opt.directory == 1 && dirp->d_type == DT_DIR) {
        strcpy(arrayOfPath[countOfPath++], path);
    }
    if(opt.file == 1 && dirp->d_type == DT_REG) {
        strcpy(arrayOfPath[countOfPath++], path);
    }
    if(opt.symlink == 1 && dirp->d_type == DT_LNK) {
        strcpy(arrayOfPath[countOfPath++], path);
    }
}



void sortPath(){
    for(int i =0; i < countOfPath - 1; i++)
        for(int j = i + 1; j < countOfPath; j++)
            if(strcmp(arrayOfPath[i], arrayOfPath[j]) > 0){ //есть траблы с большими буквами
                char temp[1024];
                strcpy(temp, arrayOfPath[i]);
                strcpy(arrayOfPath[i], arrayOfPath[j]);
                strcpy(arrayOfPath[j], temp);
            }

}

void dirwalkRecursion(char* currentDirectionPath, struct options opt) {
    DIR* currentDirection = NULL;
    if((currentDirection = opendir(currentDirectionPath)) == NULL){ //открываем каталог
        fprintf(stderr, "Невозможно открыть %s\n", currentDirectionPath);
    }
    struct dirent* dirp = NULL;
    while((dirp = readdir((currentDirection))) != NULL){    //цикл по каталогу

        if((strcmp(dirp->d_name, ".") == 0 ) || (strcmp(dirp->d_name, "..") == 0))
            continue;

        char currentFilePath[MAXSIZE];
        upgradePath(currentFilePath, currentDirectionPath, dirp->d_name);

        //вывод пути файла в зависимости от условия
        if(opt.sort == 1 && countOfPath < MAXSIZE)
            savePathToArray(currentFilePath, dirp, opt);
        else
            printPath(currentFilePath, dirp, opt);

        if (dirp->d_type == DT_DIR)
            dirwalkRecursion(currentFilePath, opt);
    }
    closedir(currentDirection);
}

void dirwalk(char* currentDirectionPath, struct options opt){
    printf("%s\n", currentDirectionPath);
    dirwalkRecursion(currentDirectionPath, opt);
    if(opt.sort == 1){
        sortPath();
        for(int i = 0; i < countOfPath; i++){
            printf("%s\n", arrayOfPath[i]);
        }
    }
}

int main(int argc, char* argv[]) {
    struct options OPT = {0, 0, 0, 0};
    char directoryPath[1024] = ".", ch;
    short flagPath = 0;

    while ((ch = getopt(argc, argv, "lfds")) != -1) {
        switch (ch) {
            case 'l':
                OPT.symlink = 1;
                break;
            case 'd':
                OPT.directory = 1;
                break;
            case 'f':
                OPT.file = 1;
                break;
            case 's':
                OPT.sort = 1;
                break;
            default:
                printf("Error options\n");
                exit(9);
        }
    }

    for(int i = 1; i < argc; i++){
        if(argv[i][0] == '-') continue;
        else if(argv[i][0] == '/' || argv[i][0] == '.'){

            if(flagPath == 1){
                fprintf(stderr, "Two Path. Error\n");
                exit(10);
            }

            strcpy(directoryPath, argv[i]);
            flagPath = 1;
        }
        else{
            fprintf(stderr, "Options error\n");
            exit(10);
        }
    }

    if(OPT.file == 0 && OPT.directory == 0 && OPT.symlink == 0) {
        OPT.symlink = 1;
        OPT.directory = 1;
        OPT.file = 1;
    }

    dirwalk(directoryPath, OPT);
    return 0;
}