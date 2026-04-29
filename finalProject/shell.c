void showHistory(char*[]);
void addHistory(char*[], char*);
void clearHistory(char*[]);
void createFile(char*, char*);
void executeCommand(char*, char*);
void parseCommand(char*, char*, char*, char*);
void clearScreen(int);
void getHelp(void);
int getStringLength(char*);
int stringEquals(char*, char*);
int getStringLength(char*);
int div(int, int);

void main(){
    
    char line[80];
    char buffer[13312];
    char operation[32], arg1[32], arg2[32];
    char* history[80];

    clearHistory(history);

    clearScreen(24);

    interrupt(0x21, 0, "Welcome to my OS!\0", 1, 0);
    interrupt(0x21, 0, "\0", 1, 0);
    getHelp();
    interrupt(0x21, 0, "\0", 1, 0);

    while (1){
        
        interrupt(0x21, 0, ">\0", 0, 0);
        interrupt(0X21, 1, line, 0, 0);

        parseCommand(line, operation, arg1, arg2);

        if(stringEquals(operation, "cat")){

            interrupt(0x21, 3, arg1, buffer, 0);
            interrupt(0x21, 0, buffer, 0, 0);

        }else if (stringEquals(operation, "touch") == 1){

            createFile(buffer, arg1);

        }else if (stringEquals(operation, "mv") == 1){

            interrupt(0x21, 3, arg1, buffer, 0);
            interrupt(0x21, 8, arg2, buffer, div(getStringLength(buffer), 512) + 1);
            interrupt(0x21, 7, arg1, 0, 0);

        }else if (stringEquals(operation, "rm") == 1){

            interrupt(0x21, 7, arg1, 0, 0);

        }else if (stringEquals(operation, "cp") == 1){

            interrupt(0x21, 3, arg1, buffer, 0);
            interrupt(0x21, 8, arg2, buffer, div(getStringLength(buffer), 512) + 1);

        }else if (stringEquals(operation, "execute") == 1){

            interrupt(0x21, 4, arg1, 0x2000, 0);

        }else if (stringEquals(operation, "ls") == 1){

            interrupt(0x21, 9, 0, 0, 0);

        }else if (stringEquals(operation, "clear") == 1){

            clearScreen(24);

        }else if (stringEquals(operation, "history") == 1){

            showHistory(history);

        }else if (stringEquals(operation, "/help") == 1){

            getHelp();

        }else{

            interrupt(0x21, 0, "Invalid Command (/help for more info)\0", 1, 0);

        }

        addHistory(history, line);

    }
    


}


void showHistory(char* history[80]){
    int i;
    char temp[2];
    temp[1] = '\0';

    for(i = 0; i < 10; i++){
        if(getStringLength(history[i]) == 0){
            break;
        }
        
        temp[0] = (char) i + 0x30;
        interrupt(0x21, 0, temp, 0, 0);
        interrupt(0x21, 0, ": \0", 0, 0);
        interrupt(0x21, 0, history[i], 1, 0);
    }
}

void addHistory(char* history[80], char* line){
    int i;
    int j;
    char* temp;


    for(i = 0; i < 10; i++){
        if(getStringLength(history[i]) == 0){


            for(j = 0; j < getStringLength(line); j++){
                temp[j] = line[j];
            }

            history[i] = temp;

            return;
        }
    }
}


void clearHistory(char* history[80]){
    int i;
    for(i = 0; i < 10; i++){
        history[i] = "\0";
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

void getHelp(){
    interrupt(0x21, 0, "Available Commands: \0", 1, 0);
    interrupt(0x21, 0, "cat <file> | Show content of a file.\0", 1, 0);
    interrupt(0x21, 0, "touch <file> | Create a new file. \0", 1, 0);
    interrupt(0x21, 0, "mv <file1> <file2> | Move file1 to file2.\0", 1, 0);
    interrupt(0x21, 0, "rm <file> | Remove a file.\0", 1, 0);
    interrupt(0x21, 0, "cp <file1> <file2> | Copy contents of file1 to a new file2.\0", 1, 0);
    interrupt(0x21, 0, "execute <file> | Execute a file.\0", 1, 0);
    interrupt(0x21, 0, "ls | Show all files.\0", 1, 0);
    interrupt(0x21, 0, "clear | Clears the screen.\0", 1, 0);
    interrupt(0x21, 0, "history | Show command history.\0", 1, 0);
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