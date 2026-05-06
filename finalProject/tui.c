#include "shell.h"
#include "history.h"
#include "string.h"

void executeCommandTUI(char*, char[][], char*, char*, char*);
void takeTextInput(char*, char*);
void manageCursor(int);
void selectFile(char*);
void selectCommand(char*);
int getCommands(char[][32]);

char buffer[13312];

void main() {
    char line[80];
    char operation[32], arg1[32], arg2[32];
    char history[10][80];
    int i;

    clearHistory(history);
    clearScreen();

    interrupt(0x21, 0, "Welcome to my OS!", 1, 0);
    interrupt(0x21, 0, "", 1, 0);


    while (1) {

        clearString(operation, 32);
        clearString(arg1, 32);
        clearString(arg2, 32);
        clearString(line, 80);
        
        selectCommand(operation);

        executeCommandTUI(buffer, history, operation, arg1, arg2);
    }
}

void executeCommandTUI(char* buffer, char history[10][80], char* operation, char* arg1, char* arg2){
        
    if(stringEquals(operation, "cat")){

        selectFile(arg1);

    }else if (stringEquals(operation, "touch")){

        takeTextInput("New Filename: ", arg1);

    }else if (stringEquals(operation, "mv")){

        selectFile(arg1);
        takeTextInput("New Filename: ", arg2);

    }else if (stringEquals(operation, "rm")){

        selectFile(arg1);

    }else if (stringEquals(operation, "cp")){

        selectFile(arg1);

        takeTextInput("New Filename: ", arg2);

    }else if (stringEquals(operation, "execute")){

        selectFile(arg1);

    }else if (stringEquals(operation, "ls")){

        

    }else if (stringEquals(operation, "clear")){

        

    }else if (stringEquals(operation, "history")){

        

    }else if (stringEquals(operation, "!")){

        takeTextInput("#: ", arg1);

    }else if (stringEquals(operation, "/help")){

        

    }

    executeCommand(buffer, history, operation, arg1, arg2);

    /* pause screen */
    if (stringEquals(operation, "cat") || stringEquals(operation, "ls") || 
        stringEquals(operation, "history") || stringEquals(operation, "/help")) {
        
        
        interrupt(0x21, 0, "", 1, 0);
        interrupt(0x21, 0, "Press any key to return to menu", 1, 0);
        interrupt(0x16, 0, 0, 0, 0);
    }
}

void takeTextInput(char* header, char* arg){
    clearString(arg, 32);
    clearScreen();
    interrupt(0x21, 0, header, 0, 0);
    interrupt(0x21, 1, arg, 0, 0);
}


void manageCursor(int index){
    char key = interrupt(0x16, 0, 0, 0, 0);

    if(key == 'w'){        
        index--;
    }else if (key == 's'){        
        index++;
    }
}

void selectFile(char* fileName) {
    char entryNames[16][7];
    int i;
    int cursorIndex = 0;
    int prevIndex = 0;
    char key;
    int length;
    int change = 1;

    /*clear*/
    for(i = 0; i < 16; i++){
        clearString(entryNames[i], 7);
    }

    /*get files*/
    interrupt(0x21, 9, entryNames, 0, 0);

    clearScreen();
    interrupt(0x21, 0, "Select File: ", 1, 0);

    for(i = 0; i < 16; i++){
        if(entryNames[i][0] == 0x00){
            length = i;
            break;
        }
        interrupt(0x21, 0, "   ", 0, 0);
        interrupt(0x21, 0, entryNames[i], 1, 0);
    }


    while(1){

        if(change){
            moveCursor(prevIndex + 1, 0);
            interrupt(0x21, 0, "   ", 0, 0);

            moveCursor(cursorIndex + 1, 0);
            interrupt(0x21, 0, " > ", 0, 0);
            
            prevIndex = cursorIndex;
            change = 0;
        }

        key = interrupt(0x16, 0, 0, 0, 0);

        if(key == 'w' && cursorIndex > 0) {
            cursorIndex--;
            change = 1;
        }else if(key == 's' && cursorIndex < (length - 1)){
            cursorIndex++;
            change = 1;
        }else if(key == 0xd){
            copyString(entryNames[cursorIndex], fileName);
            clearScreen();
            return;
        }

    }

    
}

void selectCommand(char* command) {
    char commands[11][32];

    int cursorIndex = 0;
    int prevIndex = 0;
    int i;
    char key;
    int totalCommands = getCommands(commands);
    int change = 1;

    clearScreen();
    interrupt(0x21, 0, "Select Command: ", 1, 0);

    /*prints all cmds*/
    for(i = 0; i < totalCommands; i++) {
        interrupt(0x21, 0, "   ", 0, 0);
        interrupt(0x21, 0, commands[i], 1, 0);
    }

    while (1)
    {
        if(change){
            moveCursor(prevIndex + 1, 0);
            interrupt(0x21, 0, "   ", 0, 0);

            moveCursor(cursorIndex + 1, 0);
            interrupt(0x21, 0, " > ", 0, 0);
            
            prevIndex = cursorIndex;
            change = 0;
        }

        key = interrupt(0x16, 0, 0, 0, 0);

        if(key == 'w' && cursorIndex > 0) {
            cursorIndex--;
            change = 1;
        }else if(key == 's' && cursorIndex < (totalCommands - 1)){
            cursorIndex++;
            change = 1;
        }else if(key == 0xd){
            copyString(commands[cursorIndex], command);
            clearScreen();
            return;
        }
        
    }

}

int getCommands(char commands[][32]){

    int length = 0;

    copyString("cat", commands[length]);
    length++;
    copyString("touch", commands[length]);
    length++;
    copyString("mv", commands[length]);
    length++;
    copyString("rm", commands[length]);
    length++;
    copyString("cp", commands[length]);
    length++;
    copyString("execute", commands[length]);
    length++;
    copyString("ls", commands[length]);
    length++;
    copyString("clear", commands[length]);
    length++;
    copyString("history", commands[length]);
    length++;
    copyString("!", commands[length]);
    length++;
    copyString("/help", commands[length]);
    length++;

    return length;
}