#include <stdio.h>
#include <unistd.h>

void cd(char PATH[], char COMMAND[]);
void checkAndDecide(char PATH[], char COMMAND[]);
void setDefaultPath(char PATH[]);


int main() {
    char USER[] = "Goora993";
    char PATH[10000];
    char SIGN = '$';
    char COMMAND[1000000];

    setDefaultPath(PATH);
    printf("%s [%s]%c ", USER, PATH, SIGN);
    gets(COMMAND);
    checkAndDecide(PATH, COMMAND);
    printf("%s [%s]%c ", USER, PATH, SIGN);


    return 0;
}

void checkAndDecide(char PATH[], char COMMAND[]){
    if(COMMAND[0]=='c' && COMMAND[1]=='d'){
        cd(PATH, COMMAND);
    }
}

void cd(char PATH[], char COMMAND[]){
    for (int i = 0; i < 100000; i++) {
        PATH[i] = COMMAND[i+3];
    }
}

void setDefaultPath(char PATH[]){
    if (getcwd(PATH, 10000) != NULL) {

    } else {
        perror("getcwd() error");
    }
}
