#include "shell.h"
#include "string.h"

void createFile(char* buffer, char* filename) {
    char line[80];
    int pos = 0;
    int i;

    interrupt(0x21, 0, "Enter text (empty line to end):\r\n", 0, 0);

    while(1) {
        clearString(line, 80);
        interrupt(0x21, 1, line, 0, 0);

        if(line[0] == 0x0) {
            break;
        }

        pos = getStringLength(buffer);
        copyString(line, &buffer[pos]);

        pos = getStringLength(buffer);
        buffer[pos] = 0xa;
        buffer[pos + 1] = 0xd;
        buffer[pos + 2] = 0x0;

    }


    interrupt(0x21, 8, filename, buffer);
}

void executeCommand(char* buffer, char history[10][80], char* operation, char* arg1, char* arg2){
    int index;
    int fileExists;


    if(stringEquals(operation, "cat")){

        if(checkFileStatus(arg1, 1) == 0){
            return;
        }

        clearString(buffer, 13312);
        interrupt(0x21, 3, arg1, buffer, 0);
        interrupt(0x21, 0, buffer, 0, 0);

    }else if (stringEquals(operation, "touch")){

        if(checkFileStatus(arg1, 0) == 0){
            return;
        }

        clearString(buffer, 13312);
        createFile(buffer, arg1);

    }else if (stringEquals(operation, "mv")){

        if(checkFileStatus(arg1, 1) == 0 || checkFileStatus(arg2, 0) == 0){
            return;
        }

        clearString(buffer, 13312);
        interrupt(0x21, 3, arg1, buffer, 0);
        interrupt(0x21, 0, "read file\r\n", 0, 0);
        interrupt(0x21, 8, arg2, buffer);
        interrupt(0x21, 0, "write file\r\n", 0, 0);
        interrupt(0x21, 7, arg1, 0, 0);
        interrupt(0x21, 0, "delete file\r\n", 0, 0);

    }else if (stringEquals(operation, "rm")){
        if(checkFileStatus(arg1, 1) == 0){
            return;
        }

        interrupt(0x21, 7, arg1, 0, 0);

    }else if (stringEquals(operation, "cp")){

        if(checkFileStatus(arg1, 1) == 0 || checkFileStatus(arg2, 0) == 0){
            return;
        }

        clearString(buffer, 13312);
        interrupt(0x21, 3, arg1, buffer, 0);
        interrupt(0x21, 8, arg2, buffer);

    }else if (stringEquals(operation, "execute")){

        if(checkFileStatus(arg1, 1) == 0){
            return;
        }

        interrupt(0x21, 4, arg1, 0x2000, 0);

    }else if (stringEquals(operation, "ls")){

        printAllEntries();

    }else if (stringEquals(operation, "clear")){

        clearScreen();

    }else if (stringEquals(operation, "history")){

        showHistory(history);

    }else if (stringEquals(operation, "!")){

        index = (int)arg1[0] - '0';

        if(index < 0 || index > 9 || history[index][0] == 0x0){
            interrupt(0x21, 0, "Invalid history index\r\n", 0, 0);
            return;
        }

        parseCommand(history[index], operation, arg1, arg2);
        executeCommand(buffer, history, operation, arg1, arg2);

    }else if (stringEquals(operation, "/help")){

        getHelp();

    }else if (operation[0] == 0x0){

        return;

    }else{

        interrupt(0x21, 0, "Invalid Command (/help for more info)\r\n", 0, 0);

    }
        
}

void parseCommand(char* cmd, char* operation, char* arg1, char* arg2) {
    int pos = 0;

    // Get the first word (the operation)
    pos = getToken(cmd, pos, operation);

    // Get the first operand
    pos = getToken(cmd, pos, arg1);

    // Get the second operand
    pos = getToken(cmd, pos, arg2);
}

int getToken(char* src, int start, char* dest) {
    int i = start;
    int j = 0;

    clearString(dest, 32);

    // 1. Skip any leading spaces
    while (src[i] == ' ') {
        i++;
    }

    // 2. Copy characters until we hit a delimiter (space, newline, or null)
    while (src[i] != 0x0 && src[i] != ' ' && src[i] != '\n' && src[i] != '\r' &&  src[i] != 0xa && src[i] != 0xd) {
        dest[j] = src[i];
        i++;
        j++;
    }

    // 3. Null-terminate the destination string
    dest[j] = 0x0;

    // 4. Return the new index so the caller knows where to start the next token
    return i;
}

void clearScreen(){
    interrupt(0x10, 0x0600, 0x0700, 0x0000, 0x184f);
    moveCursor(0, 0);
}

int checkFileStatus(char* fileName, int status){
    int fileExists;
    interrupt(0x21, 10, fileName, &fileExists, 0);

    if(fileExists == 1 && status == 1){
        return 1; 
    }else if(fileExists == 0 && status == 0){
        return 1;
    }else if (fileExists == 1 && status == 0){
        interrupt(0x21, 0, "File already exists: ", 0, 0);
        interrupt(0x21, 0, fileName, 1, 0);
        return 0;
    }else if (fileExists == 0 && status == 1){
        interrupt(0x21, 0, "File not found: ", 0, 0);
        interrupt(0x21, 0, fileName, 1, 0);
        return 0;
    }
    return 0;
}

void printAllEntries(){
    char entryNames[16][7];
    int i;

    for(i = 0; i < 16; i++){
        clearString(entryNames[i], 7);
    }

    interrupt(0x21, 9, entryNames, 0, 0);

    for(i = 0; i < 16; i++){
        if(entryNames[i][0] == 0x00){
            continue;
        }

        interrupt(0x21, 0, entryNames[i], 1, 0);
    }
}

void getHelp(){
    interrupt(0x21, 0, "Available Commands (/help): ", 1, 0);
    interrupt(0x21, 0, "cat <file> | Show content of a file.", 1, 0);
    interrupt(0x21, 0, "touch <file> | Create a new file. ", 1, 0);
    interrupt(0x21, 0, "mv <file1> <file2> | Move file1 to file2.", 1, 0);
    interrupt(0x21, 0, "rm <file> | Remove a file.", 1, 0);
    interrupt(0x21, 0, "cp <file1> <file2> | Copy contents of file1 to a new file2.", 1, 0);
    interrupt(0x21, 0, "execute <file> | Execute a file.", 1, 0);
    interrupt(0x21, 0, "ls | Show all files.", 1, 0);
    interrupt(0x21, 0, "clear | Clears the screen.", 1, 0);
    interrupt(0x21, 0, "history | Show command history.", 1, 0);
    interrupt(0x21, 0, "! <index> | Re-execute a command from history.", 1, 0);
    interrupt(0x21, 0, "/help | Show all available commands.", 1, 0);
}

void moveCursor(int row, int col){
    int pos = (row << 8) + col;
    int ax = 0x0200;

    interrupt(0x10, ax, 0, 0 , pos);
}

void errorPause(char* message){
    interrupt(0x21, 0, "\r\n", 0, 0);
    interrupt(0x21, 0, message, 1, 0);
    interrupt(0x21, 0, "\r\n", 0, 0);
}