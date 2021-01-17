#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>

void cd(char *path, char *actualPath, char* pathTmp, const char *command);

void checkAndDecide(char *path, char *actualPath, char* pathTmp, char *command);

void setDefaultPath(char *path);

void setHomePath(char* actualPath);

int ifCommandExit(const char *command);

int checkCommand(const char *command, const char *program);

void ls(const char *actualPath);

void pwd(char *path);

void clearPathTmp(char* pathTmp);



int BUFFER_SIZE = 10000;
char* HOME_PATH;

int main() {

    char *user = malloc(BUFFER_SIZE * sizeof(char));
    char *path = malloc(BUFFER_SIZE * sizeof(char));
    char *actualPath = malloc(BUFFER_SIZE * sizeof(char));
    char* pathTmp = malloc(BUFFER_SIZE * sizeof(char));
    char SIGN = '$';
    char *command = malloc(BUFFER_SIZE * sizeof(char));

    setHomePath(actualPath);

    getlogin_r(user, sizeof(user));


//app main loop
    while (ifCommandExit(command) == 0) {
        printf("%s: %s%c ", user, actualPath, SIGN);
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
        ls(actualPath);
    } else if (checkCommand(command, "pwd") != -1) {
        pwd(path);
    }
}


void setHomePath(char* actualPath) {
    if((HOME_PATH = getenv("HOME")) == NULL) {
        HOME_PATH = getpwuid(getuid())->pw_dir;
    }

    chdir(HOME_PATH);

    int homePathLength = strlen(HOME_PATH);


    for (int i = 0; i < homePathLength; ++i) {
        actualPath[i] = HOME_PATH[i];
    }
}


void cd(char *path, char *actualPath, char* pathTmp, const char *command) {
    int loopFlag = 0;
    getcwd(actualPath, BUFFER_SIZE);

    for (int i = 0; i < BUFFER_SIZE; i++) {
        path[i] = command[i + 3];
        pathTmp[i] = actualPath[i];
    }

    DIR *localPath = opendir(path);

    if (localPath != NULL) {
        if (path[0] == '/') {
            strncpy(actualPath, path, BUFFER_SIZE);
            chdir(path);
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
            if(actualPath[1]==':' && actualPath[2]==0){
                strcat(actualPath, "/");
            }
            chdir(actualPath);
        } else {
            strcat(actualPath, "/");
            strcat(actualPath, path);
            chdir(actualPath);
        }
    } else {
        printf("Brak takiej scizeki\n");
    }
}

void ls(const char *actualPath) {
    struct dirent *file;
    DIR *localPath;

    if ((localPath = opendir(actualPath))) {
        while ((file = readdir(localPath)))
            puts(file->d_name);

        closedir(localPath);
    } else
        printf("! wywołujac funkcje opendir(%s) pojawil sie blad otwarcia strumienia dla danej sciezki, moze nie istnieje, "
               "lub podano sciezkę pusta\n", actualPath);

}

void pwd(char *path) {
    printf("%s\n", path);
}


int checkCommand(const char *command, const char *program) {
    for (int i = 0; program[i]; i++) {
        if (command[i] != program[i])
            return -1;
    }
    return 0;
}

int ifCommandExit(const char *command) {
    if (command[0] == 'e' && command[1] == 'x' && command[2] == 'i' && command[3] == 't') {
        return 1;
    } else {
        return 0;
    }
}

void clearPathTmp(char* pathTmp){
    pathTmp[0] = 0;
}

