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
    interrupt(0x21, 0, line, 1, 0);

    parseCommand(line, operation, operand);
        /* 
    interrupt(0x21, 0, operation, 1, 0);

    interrupt(0x21, 0, operand, 1, 0);
        */
    executeCommand(operation, operand);

}

void executeCommand(char* operation, char* operand){
    char buffer[13312];

    if(stringEquals(operation, "type\0")){
        interrupt(0x21, 0, "running type()\0", 1, 0);
        
        interrupt(0x21, 3, operand, buffer, 0);
        interrupt(0x21, 0, buffer, 0, 0);
        /* 
        
        */
    }else if (stringEquals(operation, "execute\0") == 1){
        interrupt(0x21, 0, "running execute()\0", 1, 0);
        interrupt(0x21, 4, operand, 0x2000, 0);

    }else{
        interrupt(0x21, 0, "invalid command\0", 1, 0);
    }
}

void parseCommand(char* cmd, char* operation, char* operand){

    int index = 0;
    int operationIndex = 0;
    int operandIndex = 0;

    while(cmd[index] != 0x0){
        if(cmd[index] == 0x20){
            operation[operationIndex] = 0x0;
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
    operand[operandIndex] = 0x0;
 
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