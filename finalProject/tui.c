#include "shell.h"
#include "history.h"
#include "string.h"

void moveCursor(int, int);
void manageCursor(int);
void selectFile(char*);
void selectCommand(char*);

void main() {
    char line[80];
    char buffer[13312];
    char operation[32], arg1[32], arg2[32];
    char history[10][80];
    int i;

    clearHistory(history);
    clearScreen(24);

    while (1) {
        // 1. Reset all buffers
        for(i = 0; i < 32; i++) { operation[i] = 0; arg1[i] = 0; arg2[i] = 0; }
        for(i = 0; i < 80; i++) { line[i] = 0; }

        // 2. Selection Menu
        selectCommand(operation);

        // 3. Prepare Arguments based on the Command
        
        // Commands requiring 1 File from Menu
        if (stringEquals(operation, "cat") || stringEquals(operation, "execute") || stringEquals(operation, "rm")) {
            selectFile(arg1);
        }
        
        // Commands requiring a typed New Name (Touch)
        else if (stringEquals(operation, "touch")) {
            interrupt(0x21, 0, "New Filename: \0", 0, 0);
            interrupt(0x21, 1, line, 0, 0);
            getToken(line, 0, arg1); // Extract name, strip newline/spaces
        }

        // Commands requiring 1 Menu File + 1 Typed Name (MV/CP)
        else if (stringEquals(operation, "mv") || stringEquals(operation, "cp")) {
            interrupt(0x21, 0, "Select Source:\r\n\0", 0, 0);
            selectFile(arg1);
            
            interrupt(0x21, 0, "Enter Destination Name: \0", 0, 0);
            interrupt(0x21, 1, line, 0, 0);
            getToken(line, 0, arg2); // Extract name, strip newline/spaces
        }

        // 4. Centralized Execution
        // Note: For 'ls', 'history', 'clear', and '/help', arg1/arg2 remain empty strings
        clearScreen(24);
        executeCommand(buffer, history, operation, arg1, arg2);

        // 5. Post-Execution Pause
        if (stringEquals(operation, "cat") || stringEquals(operation, "ls") || 
            stringEquals(operation, "history") || stringEquals(operation, "/help")) {
            interrupt(0x21, 0, "\r\nPress any key to return to menu...\0", 0, 0);
            interrupt(0x16, 0, 0, 0, 0);
        }
    }
}


void moveCursor(int row, int col){
    int pos = (row << 8) + col;
    int ax = 0x0200;

    interrupt(0x10, ax, 0, 0 , pos);

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
    char key;
    int lastIndex;

    /*clear*/
    for(i = 0; i < 16; i++){
        entryNames[i][0] = 0x0;
    }

    /*get files*/
    interrupt(0x21, 9, entryNames, 0, 0);

    /*get last*/
    for(i = 0; i < 16; i++){
        if(entryNames[i][0] == 0x00){
            lastIndex = i - 1;
            break;
        }
    }

    while(1){
        clearScreen(24);
        moveCursor(0, 0);

        interrupt(0x21, 0, "Select File: \0", 1, 0);

        for(i = 0; i < 16; i++){
            if(entryNames[i][0] == 0x00){
                continue;
            }

            if(cursorIndex == i){
                interrupt(0x21, 0, "> \0", 0, 0);
            }

            interrupt(0x21, 0, entryNames[i], 1, 0);
        }

        key = interrupt(0x16, 0, 0, 0, 0);
        if(key == 'w'){        
            cursorIndex--;
        }else if (key == 's'){        
            cursorIndex++;
        }else if (key == 0xd){
            for(i = 0; i < 7; i++) {
                fileName[i] = entryNames[cursorIndex][i];
            }
            return;
        }

        
        if(cursorIndex < 0){
            cursorIndex = 0;
        }else if(cursorIndex >= lastIndex){
            cursorIndex = lastIndex;
        }

    }

    
}




void selectCommand(char* command) {
    char options[10][10]; // 10 commands, max 10 chars each
    int cursorIndex = 0;
    int i, j;
    char key;
    int totalCommands = 8; // Adjust based on how many you implement

    // Row 0: cat
    options[0][0]='c'; options[0][1]='a'; options[0][2]='t'; options[0][3]='\0';
    // Row 1: touch
    options[1][0]='t'; options[1][1]='o'; options[1][2]='u'; options[1][3]='c'; options[1][4]='h'; options[1][5]='\0';
    // Row 2: mv
    options[2][0]='m'; options[2][1]='v'; options[2][2]='\0';
    // Row 3: rm
    options[3][0]='r'; options[3][1]='m'; options[3][2]='\0';
    // Row 4: cp
    options[4][0]='c'; options[4][1]='p'; options[4][2]='\0';
    // Row 5: execute
    options[5][0]='e'; options[5][1]='x'; options[5][2]='e'; options[5][3]='c'; options[5][4]='u'; options[5][5]='t'; options[5][6]='e'; options[5][7]='\0';
    // Row 6: ls
    options[6][0]='l'; options[6][1]='s'; options[6][2]='\0';
    // Row 7: clear
    options[7][0]='c'; options[7][1]='l'; options[7][2]='e'; options[7][3]='a'; options[7][4]='r'; options[7][5]='\0';

    while(1) {
        clearScreen(24);
        moveCursor(0, 0);
        interrupt(0x21, 0, "Select Command: \0", 1, 0);

        for(i = 0; i < totalCommands; i++) {
            if(cursorIndex == i) {
                interrupt(0x21, 0, " > \0", 0, 0);
            } else {
                interrupt(0x21, 0, "   \0", 0, 0);
            }
            interrupt(0x21, 0, options[i], 1, 0);
        }

        key = interrupt(0x16, 0, 0, 0, 0);
        
        if(key == 'w' && cursorIndex > 0) cursorIndex--;
        else if(key == 's' && cursorIndex < (totalCommands - 1)) cursorIndex++;
        else if(key == 0x0d) { // ENTER
            for(j = 0; j < 10; j++) {
                command[j] = options[cursorIndex][j];
            }
            return;
        }
    }
}