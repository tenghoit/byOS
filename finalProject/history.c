#include "history.h"
#include "string.h"

void showHistory(char history[10][80]){
    int i;
    char temp[2];
    temp[1] = '\0';

    for(i = 0; i < 10; i++){
        if(history[i][0] == 0x0){
            break;
        }
        
        temp[0] = (char) i + '0';
        interrupt(0x21, 0, temp, 0, 0);
        interrupt(0x21, 0, ": \0", 0, 0);
        interrupt(0x21, 0, history[i], 0, 0);
    }
}

void addHistory(char history[10][80], char* line){
    int i;
    int j;

    if(history[9][0] != 0x0){
        removeHistory(history, 0);
    }

    for(i = 0; i < 10; i++){
        if(history[i][0] == 0x0){

            for(j = 0; j < getStringLength(line); j++){
                history[i][j] = line[j];
            }

            return;
        }
    }
}

void removeHistory(char history[10][80], int index){
    int i;
    int j;

    for(i = index; i < 10; i++){

        if(i + 1 >= 10){
            history[i][0] = 0x0;
            break;
        }

        for(j = 0; j < 80; j++){
            history[i][j] = history[i + 1][j];
        }
    }
}


void clearHistory(char history[10][80]){
    int i;
    int j;
    for(i = 0; i < 10; i++){
        for(i = 0; i < 80; i++){
            history[i][j] = 0x0;    
        }
    }
}