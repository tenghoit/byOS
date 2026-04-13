void printString(char*);
void readString(char*);
void readSector(char*, int);
int mod(int, int);
int div(int, int);

void makeInterrupt21(void);

void main(){

    /*
    char buffer[512];
    readSector(buffer, 30);
    printString(buffer);
    */

    char line[80];
    makeInterrupt21();



    while (1)
    {
        /*  
        char line[80];
        printString("Enter a line: \0");
        readString(line);
        printString(line);
        interrupt(0x10,0xe*256+0xa, 0, 0, 0);
        interrupt(0x10,0xe*256+0xd, 0, 0, 0);
        */

        interrupt(0x21, 1, line, 0, 0);
        interrupt(0x21, 0, line, 0, 0);
        interrupt(0x10,0xe*256+0xa, 0, 0, 0);
        interrupt(0x10,0xe*256+0xd, 0, 0, 0);

    }
    
}

void printString(char* str){
    char al = 'Q';
    char ah = 0xe;

    int index = 0;

    while(str[index] != 0x0){
        int ax = ah * 256 + str[index];
        interrupt(0x10, ax, 0, 0, 0);
        index++;
    }
}

void readString(char* str){
    int index = 0;
    char enterKey = 0xd;

    while(1){

        char c = interrupt(0x16, 0, 0, 0, 0);

        if (c == enterKey){
            str[index] = 0xa;
            index++;
            str[index] = 0x0;
            break;
        }

        str[index] = c;
        index++;
    }    
}

void readSector(char* buffer, int sector){
    int ah = 2;
    int al = 1;
    int ax = ah * 256 + al;

    int ch;
    int cl = mod(sector, 18) + 1;
    int cx = ch * 256 + cl;

    int dh = mod(div(sector, 18), 2);
    int dl = 0;
    int dx = dh * 256 + dl;

    interrupt(0x13, ax, buffer, cx, dx);
}

int mod(int a, int b){
    while (a >= b){
        a = a - b;
    }
    return a;
}

int div(int a, int b){
    int quotient = 0;
    while (quotient * b <= a){
        quotient++;
    }
    return quotient - 1;
}




void handleInterrupt21(int ax, int bx, int cx, int dx){

    if(ax == 0){
        printString(bx);
    }else if(ax == 1){
        readString(bx);
    }else if (ax == 2){
        readSector(bx, cx);
    }else{
        printString("Invalid interrupt!\0");
    }
}