#include "history.h"
#include "string.h"

#define lineWidth 80
#define totalHistory 10

void showHistory(char history[totalHistory][lineWidth]){
    int i;
    char temp[2];
    temp[1] = '\0';

    for(i = 0; i < totalHistory; i++){
        if(history[i][0] == 0x0){
            break;
        }
        
        temp[0] = (char) i + '0';
        interrupt(0x21, 0, temp, 0, 0);
        interrupt(0x21, 0, ": \0", 0, 0);
        interrupt(0x21, 0, history[i], 1, 0);
    }
}

void addHistory(char history[totalHistory][lineWidth], char* line){
    int i;
    int j;

    if(history[totalHistory - 1][0] != 0x0){
        removeHistory(history, 0);
    }

    for(i = 0; i < totalHistory; i++){
        if(history[i][0] == 0x0){
            clearString(history[i], lineWidth);
            copyString(line, history[i]);
            return;
        }
    }
}

void removeHistory(char history[totalHistory][lineWidth], int index){
    int i;

    for(i = index; i < totalHistory; i++){

        clearString(history[i], lineWidth);
        if(i + 1 >= totalHistory){
            break;
        }
        copyString(history[i + 1], history[i]);
    }
}


void clearHistory(char history[totalHistory][lineWidth]){
    int i;
    for(i = 0; i < totalHistory; i++){
        clearString(history[i], lineWidth);
    }
}

void rebuildLine(char* line, char* op, char* arg1, char* arg2) {
    int pos = 0;
    clearString(line, lineWidth);

    copyString(op, line);
    pos = getStringLength(line);

    if (arg1[0] != 0x0) {
        line[pos] = ' ';
        pos++;
        copyString(arg1, line + pos);
        pos = getStringLength(line);
    }

    if (arg2[0] != 0x0) {
        line[pos] = ' ';
        pos++;
        copyString(arg2, line + pos);
    }
}