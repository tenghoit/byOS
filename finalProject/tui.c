void showHistory(char[10][80]);
void addHistory(char[10][80], char*);
void removeHistory(char[10][80], int);
void clearHistory(char[10][80]);
void createFile(char*, char*);
void executeCommand(char*, char[10][80], char*, char*, char*);
void parseCommand(char*, char*, char*, char*);
int getToken(char*, int, char*);
void clearScreen(int);
int checkFileStatus(char*, int);
void printAllEntries(void);
void getHelp(void);
int getStringLength(char*);
int stringEquals(char*, char*);
int getStringLength(char*);
int div(int, int);



void moveCursor(int, int);
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






void showHistory(char history[10][80]){
    int i;
    char temp[2];
    temp[1] = '\0';

    for(i = 0; i < 10; i++){
        if(history[i][0] == 0x0){
            break;
        }
        
        temp[0] = (char) i + 0x30;
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
    for(i = 0; i < 10; i++){
        history[i][0] = 0x0;
    }
}



void createFile(char* buffer, char* filename) {
    char line[80];
    int pos = 0;
    int i;

    buffer[0] = 0x0;

    interrupt(0x21, 0, "Enter text (empty line to end):\r\n\0", 0, 0);

    while(1) {
        interrupt(0x21, 1, line, 0, 0);
        /* 
        interrupt(0x21, 0, line, 2, 0);
        */

        // In your readString, an empty line is just a Carriage Return (\xa or \xd)
        // If the first char is a newline, the user is done.
        if(line[0] == 0xa || line[0] == 0x0 || line[0] == 0xd) {
            break;
        }

        pos = getStringLength(buffer);

        // Append the new line to our big buffer
        for(i = 0; i < getStringLength(line); i++) {
            buffer[pos + i] = line[i];
        }
        
        // Ensure the buffer is null-terminated for the next getStringLength call
        buffer[pos + getStringLength(line)] = 0x0;
    }

    // Calculate sectors: (Length / 512) + 1
    interrupt(0x21, 8, filename, buffer, div(getStringLength(buffer), 512) + 1);
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
        interrupt(0x21, 8, arg2, buffer, div(getStringLength(buffer), 512) + 1);
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
        interrupt(0x21, 8, arg2, buffer, div(getStringLength(buffer), 512) + 1);

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
    while (src[i] != ' ' && src[i] != '\n' && src[i] != '\r' && src[i] != '\0') {
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

int stringEquals(char* str1, char* str2){
    int index = 0;

    if (getStringLength(str1) != getStringLength(str2)){
        return 0;
    }

    while(str1[index] != 0x0){
        if(str1[index] != str2[index]){
            return 0;
        }
        index++;
    }
    return 1;
}

int getStringLength(char* str){
    int length = 0;
    while(str[length] != 0x0){
        length++;
    }
    return length;
}

int div(int a, int b){
    int quotient = 0;
    while (quotient * b <= a){
        quotient++;
    }
    return quotient - 1;
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