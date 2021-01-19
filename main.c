#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int execProcess(char* command);

void cd(char *path, char *actualPath, char* pathTmp, const char *command);

void checkAndDecide(char *path, char *actualPath, char* pathTmp, char *command);

void setHomePath(char* actualPath);

int checkCommand(const char *command, const char *program);

void ls(char *path, char *actualPath, const char *command);

void pwd(char *path);

void clearPathTmp(char* pathTmp);



int BUFFER_SIZE = 10000;
int LOOP_FLAG = 0;
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



//app main loop
    while (LOOP_FLAG == 0) {
        printf("%s[%s] %c ", user, actualPath, SIGN);
        fgets(command, BUFFER_SIZE, stdin); //read command from stdin
        command[strcspn(command, "\n")] = 0; //remove \n sign after line with command
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
    // parent

    wait(NULL);
    return 0;
}


void cd(char *path, char *actualPath, char* pathTmp, const char *command) {
    int loopFlag = 0;
    int pathLength = 0;
    getcwd(actualPath, BUFFER_SIZE);

    for (int i = 0; i < BUFFER_SIZE; i++) {
        path[i] = command[i + 3];
        pathTmp[i] = actualPath[i];
    }

    pathLength = strlen(path);


    DIR *localPath = opendir(path);
    if(pathLength==0){
        strncpy(actualPath, HOME_PATH, BUFFER_SIZE);
        chdir(actualPath);
        printf("Actual path: %s\n", actualPath);
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
            for (int i = 0; i < BUFFER_SIZE; i++) {
                if(actualPath[BUFFER_SIZE-i-1] == '/' && loopFlag == 0){
                    actualPath[BUFFER_SIZE-i-1]=0;
                    loopFlag = 1;
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


    for (int i = 0; i < BUFFER_SIZE; i++) {
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

void setHomePath(char* actualPath) {
    if((HOME_PATH = getenv("HOME")) == NULL) {
        HOME_PATH = getpwuid(getuid())->pw_dir;
    }

    chdir(HOME_PATH);

    strncpy(actualPath, HOME_PATH, BUFFER_SIZE);
}

void pwd(char *actualPath) {
    DIR *localPath;

    actualPath = "/sadsd";
    if(localPath = opendir(actualPath)){
        printf("%s\n", actualPath);
    } else {
        perror("Error: ");
    }
}


int checkCommand(const char *command, const char *program) {
    for (int i = 0; program[i]; i++) {
        if (command[i] != program[i])
            return -1;
    }
    return 0;
}

void clearPathTmp(char* pathTmp){
    pathTmp[0] = 0;
}

