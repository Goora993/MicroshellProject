#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

void cd(char *path, char *actualPath, char* pathTmp, const char *command);

void checkAndDecide(char *path, char *actualPath, char* pathTmp, char *command);

void setDefaultPath(char *path);

int ifCommandExit(const char *command);

int checkCommand(const char *command, const char *program);

void ls(const char *actualPath);

void pwd(char *path);

void clearPathTmp(char* pathTmp);

int main() {
    char *user = malloc(10000 * sizeof(char)); //przygotowane pod użytkownika na linuksie
    char *path = malloc(10000 * sizeof(char));
    char *actualPath = malloc(10000 * sizeof(char));
    char* pathTmp = malloc(10000 * sizeof(char));
    char SIGN = '$';
    char *command = malloc(10000 * sizeof(char));

    user = "Goora";
    setDefaultPath(actualPath);  // ten i wiersz wyżej będą odpowiadały za ustalenie ścieżki domyślnej jako katalog użytkownika

    while (ifCommandExit(command) == 0) {
        printf("%s [%s]%c ", user, actualPath, SIGN);
        gets(command);
        checkAndDecide(path, actualPath, pathTmp, command);
    }

    free(user); //tutaj jest rzucane: Process finished with exit code -1073740940 (0xC0000374) gdy zamykam program "exitem"
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

void cd(char *path, char *actualPath, char* pathTmp, const char *command) {
    int loopFlag = 0;
    getcwd(actualPath, 10000);

    for (int i = 0; i < 10000; i++) {
        path[i] = command[i + 3];
        pathTmp[i] = actualPath[i];
    }

    DIR *localPath = opendir(path);

    if (localPath != NULL) {
        if (path[1] == ':') {
            strncpy(actualPath, path, 10000);
            chdir(path);
        } else if(path[0] == '.' && path[1] != '.'){
            if(path[1] == '\\'){
                strncpy(pathTmp, path+1, 9999);
                strcat(actualPath, pathTmp);
                clearPathTmp(pathTmp);
                chdir(actualPath);
            }
            chdir(actualPath);
        } else if(path[0] == '.' && path[1] == '.'){
            for (int i = 0; i < 10000; i++) {
                if(actualPath[1000-i-1] == '\\' && loopFlag == 0){
                    actualPath[1000-i-1]=0;
                    loopFlag = 1;
                }
            }
            if(actualPath[1]==':' && actualPath[2]==0){
                strcat(actualPath, "\\");
            }
            chdir(actualPath);
        } else {
            strcat(actualPath, "\\");
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

void setDefaultPath(char *path) {
    if (getcwd(path, 10000) != NULL) {
        //ok
    } else {
        perror("getcwd() error");
    }
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

