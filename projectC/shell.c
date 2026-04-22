void executeCommand(char*, char*);
void parseCommand(char*, char*, char*);
int getStringLength(char*);
char itoa(int);
int stringEquals(char*, char*);
int getStringLength(char*);

void main(){
    
    char line[80];
    char operation[32];
    char operand[32];

    interrupt(0x21, 0, ">\0", 0, 0);
    interrupt(0X21, 1, line, 0, 0);
    interrupt(0x21, 0, line, 0, 0);

    parseCommand(line, operation, operand);

        /* 
    interrupt(0x21, 0, operation, 0, 0);
    interrupt(0x10,0xe*256+0xa, 0, 0, 0);
    interrupt(0x10,0xe*256+0xd, 0, 0, 0);

    interrupt(0x21, 0, operand, 0, 0);
    interrupt(0x10,0xe*256+0xa, 0, 0, 0);
    interrupt(0x10,0xe*256+0xd, 0, 0, 0);
        */



    executeCommand(operation, operand);

    interrupt(0x10,0xe*256+0xa, 0, 0, 0);
    interrupt(0x10,0xe*256+0xd, 0, 0, 0);

}

void executeCommand(char* operation, char* operand){
    char buffer[13312];

    if(stringEquals(operation, "type\0") == 1){
        interrupt(0x21, 0, "successful cmd match to type\0", 0, 0);
        
        interrupt(0x21, 3, operand, buffer, 0);
        interrupt(0x21, 0, buffer, 0, 0);

    }else{
        interrupt(0x21, 0, "no match\0", 0, 0);
    }
}

void parseCommand(char* cmd, char* operation, char* operand){

    int index = 0;
    int operationIndex = 0;
    int operandIndex = 0;

    while(cmd[index] != 0x0){
        if(cmd[index] == 0x20){

            index++;
            break;
        }

        operation[operationIndex] = cmd[index];
        index++;
        operationIndex++;
    }

    while(cmd[index] != 0x0){
        operand[operandIndex] = cmd[index];
        index++;
        operandIndex++;
    }
 
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