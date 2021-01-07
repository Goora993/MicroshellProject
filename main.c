#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

void cd(char* path, char* actualPath, const char* command);
void checkAndDecide(char* path, char* actualPath, char* command);
void setDefaultPath(char* path);
int ifCommandExit(const char* command);
void ls(const char * path);
void pwd(char * path);


int main() {
    char* user = malloc(10000 * sizeof(char));
    char* path = malloc(10000 * sizeof(char));
    char* actualPath = malloc(10000 * sizeof(char));
    char SIGN = '$';
    char* command = malloc(10000 * sizeof(char));

    user = "Goora";
    setDefaultPath(path);

    while(ifCommandExit(command)==0){
        printf("%s [%s]%c ", user, path, SIGN);
        gets(command);
        checkAndDecide(path, actualPath, command);
    }

    free(user); //tutaj jest rzucane: Process finished with exit code -1073740940 (0xC0000374)
    free(path);
    free(command);

    return 0;
}

void checkAndDecide(char* path, char* actualPath, char* command){
    if(command[0]=='c' && command[1]=='d'){
        cd(path, actualPath, command);
    } else if(command[0]=='l' && command[1]=='s') {
        ls(path);
    } else if(command[0]=='p' && command[1]=='w' && command[2]=='d'){
        pwd(path);
    }
}

void ls( const char* path ) {
    struct dirent* file;
    DIR* localPath;

    if((localPath = opendir(path))) {
        while(( file = readdir(localPath) ) )
            puts(file->d_name);

        closedir(localPath);
    }
    else
        printf( "! wywołując funkcję opendir(%s) pojawił się błąd otwarcia strumienia dla danej ścieżki, może nie istnieje, "
                "lub podano ścieżkę pustą\n", path);

}

void cd(char* path, char* actualPath, const char* command){
   getcwd(actualPath, 10000);

    for (int i = 0; i < 10000; i++) {
        path[i] = command[i+3];
    }

    DIR * localPath = opendir(path);

    if(readdir(localPath)==NULL){
        for (int i = 0; i < 10000; i++) {
            path[i] = actualPath[i];
        }
        printf("Brak takiej scizeki\n");
    }
}

void pwd(char * path){
    printf("%s\n", path);
}

void setDefaultPath(char* path){
    if (getcwd(path, 10000) != NULL) {
        //ok
    } else {
        perror("getcwd() error");
    }
}

int ifCommandExit(const char* command){
    if(command[0]=='e' && command[1]=='x' && command[2]=='i' && command[3]=='t'){
        return 1;
    } else {
        return 0;
    }
}

