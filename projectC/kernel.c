void printString(char*);
void readString(char*);
void readSector(char*, int);
void readFile(char*, char*);
void loadFile(char*, int*);

void getEntryName(char*, char*, int);
void getEntrySectors(int*, char*, int);

int stringEquals(char*, char*);
int getStringLength(char*);
int mod(int, int);
int div(int, int);

void makeInterrupt21(void);

void main(){

    char buffer[13312];
    makeInterrupt21();

    interrupt(0x21, 3, "messag\0", buffer, 0);

    interrupt(0x21, 0, buffer, 0, 0);

    while (1)
    {
        /*
        interrupt(0x21, 1, line, 0, 0);
        interrupt(0x21, 0, line, 0, 0);
        interrupt(0x10,0xe*256+0xa, 0, 0, 0);
        interrupt(0x10,0xe*256+0xd, 0, 0, 0);
        */
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

    int ch = mod(sector, 36);
    int cl = mod(sector, 18) + 1;
    int cx = ch * 256 + cl;

    int dh = mod(div(sector, 18), 2);
    int dl = 0;
    int dx = dh * 256 + dl;

    interrupt(0x13, ax, buffer, cx, dx);
}

void readFile(char* fileName, char* buffer){

    int totalFileEntries = 16;
    int fileEntrySize = 32;
    int fileNameSize = 6;
    int fileSectors = 26;

    char dirSector[512];
    char entryName[6];
    int entrySectors[26];
    int i;

    readSector(dirSector, 2);

    for(i = 0; i < totalFileEntries; i++){
        int start = i * fileEntrySize;
        getEntryName(entryName, dirSector, i);
        if(stringEquals(fileName, entryName) == 1){
            getEntrySectors(entrySectors, dirSector, i);
            loadFile(buffer, entrySectors);
            return;
        }
    }
}

void loadFile(char* buffer, int* entrySectors){
    int fileSectors = 26;
    int i;
    for(i = 0; i < fileSectors; i++){
        readSector(buffer + (i * 512), entrySectors[i]);
    }
}

void getEntryName(char* entryName, char* dirSector, int entryIndex){
    int fileEntrySize = 32;
    int fileNameSize = 6;

    int start = entryIndex * fileEntrySize;
    int end = start + fileNameSize;

    int i;
    for(i = start; i < end; i++){
        entryName[i - start] = dirSector[i];
    }
}   

void getEntrySectors(int* entrySector, char* dirSector, int entryIndex){
    int fileEntrySize = 32;
    int fileNameSize = 6;
    int fileSectors = 26;

    int entrySectors[26];
    int start = entryIndex * fileEntrySize + fileNameSize;
    int end = start + fileSectors;

    int i;
    for(i = start; i < end; i++){
        entrySectors[i - start] = dirSector[i];
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
    }else if(ax == 3){
        readFile(bx, cx);
    }else{
        printString("Invalid interrupt!\0");
    }
}