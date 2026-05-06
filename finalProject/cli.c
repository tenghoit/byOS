#include "shell.h"
#include "history.h"
#include "string.h"

char buffer[13312];

void main(){
    
    char line[80];
    char operation[32], arg1[32], arg2[32];
    char history[10][80];

    clearHistory(history);
    clearScreen();
    clearString(buffer, 13312);

    interrupt(0x21, 0, "Welcome to my OS!", 1, 0);
    interrupt(0x21, 0, "", 1, 0);
    getHelp();
    interrupt(0x21, 0, "", 1, 0);

    while (1){
        
        clearString(operation, 32);
        clearString(arg1, 32);
        clearString(arg2, 32);
        clearString(line, 80);
        
        interrupt(0x21, 0, " > ", 0, 0);
        interrupt(0X21, 1, line, 0, 0);

        parseCommand(line, operation, arg1, arg2);
        addHistory(history, line);
        executeCommand(buffer, history, operation, arg1, arg2);

    }

}