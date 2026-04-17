
void parseCommand(char*, char*, char*);

void main(){
    
    char line[80];
    char* operation;
    char* operand;

    interrupt(0x21, 0, ">\0", 0, 0);
    interrupt(0X21, 1, line, 0, 0);
    interrupt(0x21, 0, line, 0, 0);

    interrupt(0x10,0xe*256+0xa, 0, 0, 0);
    interrupt(0x10,0xe*256+0xd, 0, 0, 0);


    parseCommand(line, operation, operand);
    interrupt(0x21, 0,  operation, 0, 0);
    interrupt(0x10,0xe*256+0xa, 0, 0, 0);
    interrupt(0x10,0xe*256+0xd, 0, 0, 0);

    interrupt(0x21, 0, operand, 0, 0);
    interrupt(0x10,0xe*256+0xa, 0, 0, 0);
    interrupt(0x10,0xe*256+0xd, 0, 0, 0);

    interrupt(0x10,0xe*256+0xa, 0, 0, 0);
    interrupt(0x10,0xe*256+0xd, 0, 0, 0);





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