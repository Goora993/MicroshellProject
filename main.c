#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void cd(char* path, const char* command);
void checkAndDecide(char* path, char* command);
void setDefaultPath(char* path);
void clearPath(char* path);
void clearCommand(char* command);
int ifCommandExit(const char* command);


int main() {
    char* user = malloc(10000 * sizeof(char));
    char* path = malloc(10000 * sizeof(char));
    char SIGN = '$';
    char* command = malloc(10000 * sizeof(char));

    user = "Goora";
    setDefaultPath(path);

    while(ifCommandExit(command)==0){
        printf("%s [%s]%c ", user, path, SIGN);
        gets(command);
        checkAndDecide(path, command);
    }

    free(user); //tutaj jest rzucane: Process finished with exit code -1073740940 (0xC0000374)
    free(path);
    free(command);

    return 0;
}

void checkAndDecide(char* path, char* command){
    if(command[0]=='c' && command[1]=='d'){
        cd(path, command);
    }
}

void cd(char* path, const char* command){
    for (int i = 0; i < 10000; i++) {
        path[i] = command[i+3];
    }
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

//void clearPath(char* path){
//    for (int i = 0; i < 10000; i++) {
//        path[i] = (char) 0;
//    }
//}
//
//void clearCommand(char* command){
//    for (int i = 0; i < 10000; i++) {
//        command[i] = (char) 0;
//    }
//}

