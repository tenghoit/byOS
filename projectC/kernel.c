void executeProgram(char*, int);
void printString(char*, int);
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

    makeInterrupt21();
    
    interrupt(0x21, 4, "shell\0", 0x2000, 0);
}

void terminate(){
    interrupt(0x21, 4, "shell\0", 0x2000, 0);
}

void executeProgram(char* name, int segment){
    char buffer[13312];
    int i;

    readFile(name, buffer);
  
    for(i = 0; i < 13312; i++){
        putInMemory(segment, i, buffer[i]);
    }

    interrupt(0x21, 0, "executing \0", 0, 0);
    interrupt(0x21, 0, name, 1, 0);
    launchProgram(segment);
}

void printString(char* str, int newLine){
    char al = 'Q';
    char ah = 0xe;

    int index = 0;

    while(str[index] != 0x0){
        int ax = ah * 256 + str[index];
        interrupt(0x10, ax, 0, 0, 0);
        index++;
    }

    if(newLine == 1){
        interrupt(0x10,0xe*256+0xa, 0, 0, 0);
        interrupt(0x10,0xe*256+0xd, 0, 0, 0);
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

    int ch = div(sector, 36);
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
    char entryName[7];
    int entrySectors[26];
    int i;
    int found = 0;

    readSector(dirSector, 2);
    
    interrupt(0x21, 0, "Files: \0", 1, 0);

    for(i = 0; i < totalFileEntries; i++){
        getEntryName(entryName, dirSector, i);

        interrupt(0x21, 0, entryName, 1, 0);
        interrupt(0x21, 0, "check\0", 1, 0);

        if(stringEquals(fileName, entryName) == 1){
            interrupt(0x21, 0, "found \0", 0, 0);
            interrupt(0x21, 0, fileName, 1, 0);

            getEntrySectors(entrySectors, dirSector, i);
            loadFile(buffer, entrySectors);

            found = 1;
            break;
        }
    }
    
    if(found == 0){
        interrupt(0x21, 0, "could not find \0", 0, 0);
        interrupt(0x21, 0, fileName, 1, 0);
    }
    
    
}

void loadFile(char* buffer, int* entrySectors){
    int fileSectors = 26;
    int i;
    for(i = 0; i < fileSectors; i++){
        readSector(buffer + (i * 512), entrySectors[i]);
    }
    interrupt(0x21, 0, "loaded file\0", 1, 0);
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
    entryName[fileNameSize] = 0x0;
}   

void getEntrySectors(int* entrySectors, char* dirSector, int entryIndex){
    int fileEntrySize = 32;
    int fileNameSize = 6;
    int fileSectors = 26;

    int start = entryIndex * fileEntrySize + fileNameSize;
    int end = start + fileSectors;

    int i;
    for(i = start; i < end; i++){
        entrySectors[i - start] = (int)(dirSector[i]);
    }
}

int stringEquals(char* str1, char* str2){
    int index = 0;
    int i;

    if (getStringLength(str1) != getStringLength(str2)){
        return 0;
    }

    /*
        while(str1[index] != 0x0){
        if(str1[index] != str2[index]){
            return 0;
        }
        index++;
    }
    */

    for(i = 0; i < getStringLength(str1); i++){
        if(str1[i] != str2[i]){
            return 0;
        }
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
        printString(bx, cx);
    }else if(ax == 1){
        readString(bx);
    }else if (ax == 2){
        readSector(bx, cx);
    }else if(ax == 3){
        readFile(bx, cx);
    }else if(ax == 4){
        executeProgram(bx, cx);
    }else if(ax == 5){
        terminate();
    }else if(ax == 6){
        dx = stringEquals(bx, cx);
    }else{
        printString("Invalid interrupt!\0", 1);
    }
}