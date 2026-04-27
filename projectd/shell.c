void createFile(char*, char*);
void executeCommand(char*, char*);
void parseCommand(char*, char*, char*, char*);
int getStringLength(char*);
int stringEquals(char*, char*);
int getStringLength(char*);
int div(int, int);

void main(){
    
    char line[80];
    char buffer[13312];
    char operation[32], arg1[32], arg2[32];

    while (1){
        
        interrupt(0x21, 0, ">\0", 0, 0);
        interrupt(0X21, 1, line, 0, 0);
        interrupt(0x21, 0, line, 2, 0);

        parseCommand(line, operation, arg1, arg2);

        if(stringEquals(operation, "type\0")){
            interrupt(0x21, 3, arg1, buffer, 0);
            interrupt(0x21, 0, buffer, 0, 0);
        }else if (stringEquals(operation, "execute\0") == 1){
            interrupt(0x21, 4, arg1, 0x2000, 0);
        }else if (stringEquals(operation, "delete\0") == 1){
            interrupt(0x21, 7, arg1, 0, 0);
        }else if (stringEquals(operation, "copy\0") == 1){
            interrupt(0x21, 3, arg1, buffer, 0);
            interrupt(0x21, 8, arg2, buffer, div(getStringLength(buffer), 512) + 1);
        }else if (stringEquals(operation, "create\0") == 1){
            createFile(buffer, arg1);
        }else if (stringEquals(operation, "dir\0") == 1){
            interrupt(0x21, 9, 0, 0, 0);
        }else{
            interrupt(0x21, 0, "invalid command\0", 1, 0);
        }/* code */
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
        interrupt(0x21, 0, line, 2, 0);

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