#include "shell.h"
#include "string.h"

void createFile(char* buffer, char* filename) {
    char line[80];
    int pos = 0;
    int i;

    buffer[0] = 0x0;

    interrupt(0x21, 0, "Enter text (empty line to end):\r\n\0", 0, 0);

    while(1) {
        interrupt(0x21, 1, line, 0, 0);

        if(line[0] == 0xa || line[0] == 0x0 || line[0] == 0xd) {
            break;
        }

        pos = getStringLength(buffer);

        for(i = 0; i < getStringLength(line); i++) {
            buffer[pos + i] = line[i];
        }
        
        buffer[pos + getStringLength(line)] = 0x0;
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

        interrupt(0x21, 3, arg1, buffer, 0);
        interrupt(0x21, 0, buffer, 0, 0);

    }else if (stringEquals(operation, "touch") == 1){

        if(checkFileStatus(arg1, 0) == 0){
            return;
        }

        createFile(buffer, arg1);

    }else if (stringEquals(operation, "mv") == 1){

        if(checkFileStatus(arg1, 1) == 0 || checkFileStatus(arg2, 0) == 0){
            return;
        }

        interrupt(0x21, 3, arg1, buffer, 0);
        interrupt(0x21, 8, arg2, buffer);
        interrupt(0x21, 7, arg1, 0, 0);

    }else if (stringEquals(operation, "rm") == 1){
        if(checkFileStatus(arg1, 1) == 0){
            return;
        }

        interrupt(0x21, 7, arg1, 0, 0);

    }else if (stringEquals(operation, "cp") == 1){

        if(checkFileStatus(arg1, 1) == 0 || checkFileStatus(arg2, 0) == 0){
            return;
        }

        interrupt(0x21, 3, arg1, buffer, 0);
        interrupt(0x21, 8, arg2, buffer);

    }else if (stringEquals(operation, "execute") == 1){

        if(checkFileStatus(arg1, 1) == 0){
            return;
        }

        interrupt(0x21, 4, arg1, 0x2000, 0);

    }else if (stringEquals(operation, "ls") == 1){

        printAllEntries();

    }else if (stringEquals(operation, "clear") == 1){

        clearScreen(24);

    }else if (stringEquals(operation, "history") == 1){

        showHistory(history);

    }else if (stringEquals(operation, "!") == 1){

        index = (int)arg1[0] - '0';

        if(index < 0 || index > 9 || history[index][0] == 0x0){
            interrupt(0x21, 0, "Invalid history index\0", 1, 0);
            return;
        }

        parseCommand(history[index], operation, arg1, arg2);
        executeCommand(buffer, history, operation, arg1, arg2);

    }else if (stringEquals(operation, "/help") == 1){

        getHelp();

    }else{

        interrupt(0x21, 0, "Invalid Command (/help for more info)\0", 1, 0);

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

    // 1. Skip any leading spaces
    while (src[i] == ' ' || src[i] == '\t') {
        i++;
    }

    // 2. Copy characters until we hit a delimiter (space, newline, or null)
    while (src[i] != ' ' && src[i] != '\n' && src[i] != '\r' && src[i] != '\0' && src[i] != 0xa && src[i] != 0xd) {
        dest[j] = src[i];
        i++;
        j++;
    }

    // 3. Null-terminate the destination string
    dest[j] = '\0';

    // 4. Return the new index so the caller knows where to start the next token
    return i;
}

void clearScreen(int lines){
    int i;
    for(i = 0; i < lines; i++){
        interrupt(0x21, 0, "\0", 1, 0);
    }
}

int checkFileStatus(char* fileName, int status){
    int fileExists;
    interrupt(0x21, 10, fileName, &fileExists, 0);

    if(fileExists == 1 && status == 1){
        return 1; 
    }else if(fileExists == 0 && status == 0){
        return 1;
    }else if (fileExists == 1 && status == 0){
        interrupt(0x21, 0, "File already exists: \0", 0, 0);
        interrupt(0x21, 0, fileName, 1, 0);
        return 0;
    }else if (fileExists == 0 && status == 1){
        interrupt(0x21, 0, "File not found: \0", 0, 0);
        interrupt(0x21, 0, fileName, 1, 0);
        return 0;
    }
    return 0;
}

void printAllEntries(){
    char entryNames[16][7];
    int i;

    for(i = 0; i < 16; i++){
        entryNames[i][0] = 0x0;
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
    interrupt(0x21, 0, "Available Commands (/help): \0", 1, 0);
    interrupt(0x21, 0, "cat <file> | Show content of a file.\0", 1, 0);
    interrupt(0x21, 0, "touch <file> | Create a new file. \0", 1, 0);
    interrupt(0x21, 0, "mv <file1> <file2> | Move file1 to file2.\0", 1, 0);
    interrupt(0x21, 0, "rm <file> | Remove a file.\0", 1, 0);
    interrupt(0x21, 0, "cp <file1> <file2> | Copy contents of file1 to a new file2.\0", 1, 0);
    interrupt(0x21, 0, "execute <file> | Execute a file.\0", 1, 0);
    interrupt(0x21, 0, "ls | Show all files.\0", 1, 0);
    interrupt(0x21, 0, "clear | Clears the screen.\0", 1, 0);
    interrupt(0x21, 0, "history | Show command history.\0", 1, 0);
    interrupt(0x21, 0, "! <index> | Re-execute a command from history.\0", 1, 0);
    interrupt(0x21, 0, "/help | Show all available commands.\0", 1, 0);
}