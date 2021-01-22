#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define CYAN_COLOR    "\x1b[36m"
#define YELLOW_COLOR    "\x1b[33m"
#define RESET_COLOR   "\x1b[0m"


int execProcess(char* command);
int checkCommand(const char *command, const char *program);

void cd(char *path, char *actualPath, char* pathTmp, const char *command);
void help();
void checkAndDecide(char *path, char *actualPath, char* pathTmp, char *command);
void setHomePath(char* actualPath);
void clearPathTmp(char* pathTmp);

void pwd(char *path);
void ls(char *path, char *actualPath, const char *command);
void touch(char* command);


int BUFFER_SIZE = 10000;
int LOOP_FLAG = 0;
int i;
char* HOME_PATH;


int main(int argc, char* argv[]) {

    char *user = malloc(BUFFER_SIZE * sizeof(char));
    char *path = malloc(BUFFER_SIZE * sizeof(char));
    char *actualPath = malloc(BUFFER_SIZE * sizeof(char));
    char* pathTmp = malloc(BUFFER_SIZE * sizeof(char));
    char SIGN = '$';
    char *command = malloc(BUFFER_SIZE * sizeof(char));

    setHomePath(actualPath);

    getlogin_r(user, sizeof(user));

/*app main loop*/
    while (LOOP_FLAG == 0) {
        printf(YELLOW_COLOR "%s" RESET_COLOR, user );
        printf(CYAN_COLOR "[%s] " RESET_COLOR, actualPath );
        printf(YELLOW_COLOR "%c " RESET_COLOR ,SIGN );
        fgets(command, BUFFER_SIZE, stdin); /*read command from stdin*/
        command[strcspn(command, "\n")] = 0; /*remove \n sign after line with command*/
        checkAndDecide(path, actualPath, pathTmp, command);
    }

    free(user);
    free(path);
    free(command);
    free(pathTmp);
    free(actualPath);

    return 0;
}



void checkAndDecide(char *path, char *actualPath, char* pathTmp, char *command) {
    if (checkCommand(command, "cd") != -1) {
        cd(path, actualPath, pathTmp, command);
    } else if (checkCommand(command, "ls") != -1) {
        ls(path, actualPath, command);
    } else if (checkCommand(command, "pwd") != -1) {
        pwd(actualPath);
    } else if (checkCommand(command, "exit") != -1) {
        LOOP_FLAG = -1;
    } else if (checkCommand(command, "touch") != -1) {
        touch(command);
    } else if (checkCommand(command, "help") != -1) {
        help();
    } else {
        execProcess(command);
    }
}




int execProcess(char* command) {
    pid_t pid = fork();
    if(pid<0){
        perror("Error: ");
        exit(1);
    }
    else if(pid==0){
        char d[4] = " ";
        int counter = 0;

        command[strcspn(command, "\n")] = 0;

        char *inputPart = strtok(command, d);
        char* args[1024];

        if(inputPart == NULL){
            args[counter] = command;
        }

        while(inputPart != NULL)
        {
            args[counter] = inputPart;
            args[counter+1] = NULL;
            inputPart = strtok(NULL, d);
            counter++;
        }

        counter = 0;

        execvp(args[0], args);
        perror("Error: ");
        exit(1);
    }


    wait(NULL);
    return 0;
}


void cd(char *path, char *actualPath, char* pathTmp, const char *command) {
    int loopFlag = 0;
    int pathLength = 0;
    getcwd(actualPath, BUFFER_SIZE);

    for (i = 0; i < BUFFER_SIZE; i++) {
        path[i] = command[i + 3];
        pathTmp[i] = actualPath[i];
    }

    pathLength = strlen(path);


    DIR *localPath = opendir(path);
    if(pathLength==0){
        strncpy(actualPath, HOME_PATH, BUFFER_SIZE);
        chdir(actualPath);
    } else if (localPath != NULL) {
        if (path[0] == '/') {
            strncpy(actualPath, path, BUFFER_SIZE);
            chdir(actualPath);
        } else if(path[0] == '.' && path[1] != '.'){
            if(path[1] == '/'){
                strncpy(pathTmp, path+1, BUFFER_SIZE-1);
                strcat(actualPath, pathTmp);
                clearPathTmp(pathTmp);
                chdir(actualPath);
            }
            chdir(actualPath);
        } else if(path[0] == '.' && path[1] == '.'){
            for (i = 0; i < BUFFER_SIZE; i++) {
                if(actualPath[BUFFER_SIZE-i-1] == '/' && loopFlag == 0){
                    actualPath[BUFFER_SIZE-i-1]=0;
                    loopFlag = 1;
                }
                if(actualPath[0] == 0){
                    strcat(actualPath, "/");
                }
            }
            chdir(actualPath);
        } else {
            strcat(actualPath, "/");
            strcat(actualPath, path);
            chdir(actualPath);
        }
    } else {
        perror("Error: ");
    }
}


void ls(char *path, char *actualPath, const char *command) {
    struct dirent *file;
    DIR *localPath;
    int pathLength = 0;
    int actualPathLength = 0;
    char* tmp;


    for (i = 0; i < BUFFER_SIZE; i++) {
        path[i] = command[i + 3];
    }

    pathLength = strlen(path);
    actualPathLength = strlen(actualPath);


    if(pathLength==0){
        if ((localPath = opendir(actualPath))) {
            while ((file = readdir(localPath)))
                puts(file->d_name);

            closedir(localPath);
        } else
            perror("Error: ");
    } else if (path[0]=='/'){
        if ((localPath = opendir(path))) {
            while ((file = readdir(localPath)))
                puts(file->d_name);

            closedir(localPath);
        } else
            perror("Error: ");
    } else {
        tmp = malloc(actualPathLength + pathLength + 1);
        strcat(tmp, actualPath);
        strcat(tmp, "/");
        strcat(tmp, path);
        if ((localPath = opendir(tmp))) {
            while ((file = readdir(localPath)))
                puts(file->d_name);

            closedir(localPath);
        } else
            perror("Error: ");
        free(tmp);
    }
}

void touch(char* command){
    char* str = malloc(BUFFER_SIZE * sizeof(char));
    strcat(str, command);
    char delim[] = " ";

    FILE * file;

    char *ptr = strtok(str, delim);

    while(ptr != NULL)
    {
        ptr = strtok(NULL, delim);

        if(ptr != NULL)
        {
            file = fopen(ptr, "w");

            if(file==NULL)
                perror("Error: ");

            fclose(file);
        }
    }

    free(str);
}


void setHomePath(char* actualPath) {
    if((HOME_PATH = getenv("HOME")) == NULL) {
        HOME_PATH = getpwuid(getuid())->pw_dir;
    }

    chdir(HOME_PATH);

    strncpy(actualPath, HOME_PATH, BUFFER_SIZE);
}


void pwd(char *actualPath) {
    DIR *localPath;

    if(localPath = opendir(actualPath)){
        printf("%s\n", actualPath);
    } else {
        perror("Error: ");
    }
}


int checkCommand(const char *command, const char *program) {
    for (i = 0; program[i]; i++) {
        if (command[i] != program[i])
            return -1;
    }
    return 0;
}


void clearPathTmp(char* pathTmp){
    pathTmp[0] = 0;
}


void help(){
    printf("\n* Microshell - final project * \n\n"
           "* Author: Dawid Gorkiewicz * \n"
           "   index no. 396720\n\n"
           "* Available functionalities *\n"
           "  > implemented 'cd' function \n "
           "    - similar to default cd command from Unix-like o.s.\n"
           "    - 'cd' - moves to the home directory\n"
           "    - 'cd .' - stays in the current directory \n"
           "    - 'cd ..' - moves to one directory up \n"
           "    - 'cd path' - moves from current directory to directory pointed by path  \n"
           "    - 'cd /path' - moves to the directory pointed by absolute path \n"
           "  > implemented 'touch' function  \n"
           "    - 'touch file_name' - creates file in the cwd\n"
           "    - 'touch file_name file2_name' - creates files in the cwd\n"
           "    - 'touch path/file_name' - creates file in the directory pointed by path \n"
           "    - 'touch path/file_name path/file2_name' - creates files in the directories pointed by paths \n"
           "    - impossible to create files with space inside the name \n"
           "  > implemented 'pwd' function \n"
           "    - 'pwd' - prints current working directory path\n"
           "  > implemented 'ls' function \n"
           "    - 'ls' - lists files in current working directory  \n"
           "    - 'ls path' - lists files in the directory pointed by path  \n"
           "  > implemented 'execProcess' function \n"
           "    - allows to use scripts and programs stored in PATH \n"
           "\n");
}


