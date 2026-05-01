void terminate(void);
void executeProgram(char*, int);
void printString(char*, int);
void readString(char*);
void readSector(char*, int);
void writeSector(char*, int);
void readFile(char*, char*);
void writeFile(char*, char*, int);
void deleteFile(char*);
void loadFile(char*, int*);

void getAllEntries(char*);
void printAllEntries(void);
void getEntryName(char*, char*, int);
void getEntrySectors(int*, char*, int);
int getEntryIndex(char*, char*);
int checkEntryExists(char*);
void insertEntry(char*, char*, int);
void removeEntry(char*, int);
void clearFileSectors(char*, int);
void clearMapSectors(char*, int*);

int stringEquals(char*, char*);
int getStringLength(char*);
int mod(int, int);
int div(int, int);

void makeInterrupt21(void);

void main(){

    makeInterrupt21();
    
    interrupt(0x21, 5, 0, 0, 0);
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
    }else if(newLine == 2){
        // We assume the string already had 0x0A, so just move Left
        interrupt(0x10, 0xe * 256 + 0xd, 0, 0, 0); 
    }
}

void readString(char* str){
    int index = 0;
    char enterKey = 0xd;
    char backspaceKey = 0x8;
    char temp[2];
    temp[1] = '\0';

    while(1){

        char c = interrupt(0x16, 0, 0, 0, 0);

        if (c == enterKey){
            str[index] = 0xa;
            index++;
            str[index] = 0xd;
            index++;
            str[index] = 0x0;
            
            interrupt(0x21, 0, "\0", 1, 0);

            break;
        }

        temp[0] = c;
        interrupt(0x21, 0, temp, 0, 0);
        
        if(c == backspaceKey){
            index--;
        }else{
            str[index] = c;
            index++;
        }
        
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

void writeSector(char* buffer, int sector){
    int ah = 3;
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

    char dirSector[512];
    int entrySectors[26];
    int targetIndex;

    readSector(dirSector, 2);

    targetIndex = getEntryIndex(dirSector, fileName);
    if(targetIndex == -1){
        interrupt(0x21, 0, "could not find \0", 0, 0);
        interrupt(0x21, 0, fileName, 1, 0);
        return;
    }
    /*
    interrupt(0x21, 0, "found \0", 0, 0);
    interrupt(0x21, 0, fileName, 1, 0);
    */

    getEntrySectors(entrySectors, dirSector, targetIndex);
    loadFile(buffer, entrySectors);
}

void writeFile(char* fileName, char* buffer, int numberOfSectors){
    int fileEntrySize = 32;
    int fileNameSize = 6;
    int fileSectors = 26;

    char dirSector[512];
    char mapSector[512];
    char entryNames[16][7];
    int targetIndex;
    int i;
    int freeSector;

    readSector(mapSector, 1);
    readSector(dirSector, 2);  

    targetIndex = getEntryIndex(dirSector, fileName);
    if(targetIndex != -1){
        interrupt(0x21, 0, "File already exists.\0", 1, 0);
        return;
    }

    targetIndex = getEntryIndex(dirSector, "\0");

    insertEntry(dirSector, fileName, targetIndex);
    clearFileSectors(dirSector, targetIndex);

    for(i = 0; i < numberOfSectors; i++){

        freeSector = getFreeSector(mapSector);
        if(freeSector == -1){
            interrupt(0x21, 0, "no free sectors\0", 1, 0);
            deleteFile(fileName);
            return;
        }

        mapSector[freeSector] = 0xFF;
        dirSector[targetIndex * fileEntrySize + fileNameSize + i] = freeSector;
        writeSector(buffer + (i * 512), freeSector);
    }

    interrupt(0x21, 6, mapSector, 1, 0);
    interrupt(0x21, 6, dirSector, 2, 0);
}

void deleteFile(char* fileName){
    char dirSector[512];
    char mapSector[512];
    int entrySectors[26];
    int targetIndex;

    readSector(mapSector, 1);
    readSector(dirSector, 2);

    targetIndex = getEntryIndex(dirSector, fileName);
    if(targetIndex == -1){
        interrupt(0x21, 0, "could not find \0", 0, 0);
        interrupt(0x21, 0, fileName, 1, 0);
        return;
    }
    /*
    interrupt(0x21, 0, "found \0", 0, 0);
    interrupt(0x21, 0, fileName, 1, 0);
    */

    getEntrySectors(entrySectors, dirSector, targetIndex);

    removeEntry(dirSector, targetIndex);
    clearFileSectors(dirSector, targetIndex);
    clearMapSectors(mapSector, entrySectors);

    interrupt(0x21, 6, mapSector, 1, 0);
    interrupt(0x21, 6, dirSector, 2, 0);
}

void loadFile(char* buffer, int* entrySectors){
    int fileSectors = 26;
    int i;
    for(i = 0; i < fileSectors; i++){
        readSector(buffer + (i * 512), entrySectors[i]);
    }
    
    /*
    interrupt(0x21, 0, "loaded file\0", 1, 0);
    */
    
}

void getAllEntries(char* entryNames){
    char dirSector[512];
    int totalFileEntries = 16;
    int i, j;
    int count = 0;
    char entryName[7];
    
    readSector(dirSector, 2);

    for(i = 0; i < totalFileEntries; i++){
        getEntryName(entryName, dirSector, i);

        if(entryName[0] == 0x00){
            continue;
        }

        for(j = 0; j < 7; j++){
            entryNames[count* 7 + j] = entryName[j];
        }

        count++;
    }
}


void printAllEntries(){
    char dirSector[512];
    int totalFileEntries = 16;
    int i;
    char entryName[7];
    
    readSector(dirSector, 2);

    for(i = 0; i < totalFileEntries; i++){
        getEntryName(entryName, dirSector, i);
        if(entryName[0] != 0x00){
            interrupt(0x21, 0, entryName, 1, 0);
        }
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

int getEntryIndex(char* dirSector, char* target){
    int totalFileEntries = 16;
    int fileEntrySize = 32;
    int fileNameSize = 6;
    int i;
    char entryName[7];

    for(i = 0; i < totalFileEntries; i++){
        getEntryName(entryName, dirSector, i);
        if(stringEquals(target, entryName) == 1){
            return i;
        }
    }
    return -1;
}

int checkEntryExists(char* target){
    char dirSector[512];
    int result;
    readSector(dirSector, 2);

    result = getEntryIndex(dirSector, target);

    if(result == -1){
        return 0;
    }else{
        return 1;
    }
}

void insertEntry(char* dirSector, char* fileName, int index){
    int fileEntrySize = 32;
    int fileNameSize = 6;
    int i;
    int offset = index * fileEntrySize;
    int nameFinished = 0;

    for(i = 0; i < fileNameSize; i++){
        if(fileName[i] == 0x0 || nameFinished == 1){
            dirSector[offset + i] = 0x00;
            nameFinished = 1;
        } else {
            dirSector[offset + i] = fileName[i];
        }
    }
    // Note: Don't set dirSector[offset + 6] to 0x00 here manually; 
    // that's where your first sector number belongs!
}

void removeEntry(char* dirSector, int index){
    int fileEntrySize = 32;

    dirSector[index * fileEntrySize] = 0x00;
    interrupt(0x21, 0, "cleared entry\0", 1, 0);
}

void clearFileSectors(char* dirSector, int index){
    int fileEntrySize = 32;
    int fileNameSize = 6;
    int fileSectors = 26;

    int i;
    for(i = 0; i < fileSectors; i++){
        dirSector[index * fileEntrySize + fileNameSize + i] = 0x00;
    }
}

void clearMapSectors(char* mapSector, int* entrySectors){
    int fileSectors = 26;
    int i;
    for(i = 0; i < fileSectors; i++){
        if(entrySectors[i] == 0x00){
            break;
        }
        mapSector[entrySectors[i] + 1] = 0x00;
    }
    interrupt(0x21, 0, "cleared sectors\0", 1, 0);
}

int getFreeSector(char* mapSector){
    int i;
    for(i = 0; i < 512; i++){
        if(mapSector[i] == 0x00){
            return i;
        }
    }
    return -1;
}

int stringEquals(char* str1, char* str2){
    int index = 0;
    int i;

    if (getStringLength(str1) != getStringLength(str2)){
        return 0;
    }

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
        writeSector(bx, cx);
    }else if(ax == 7){
        deleteFile(bx);
    }else if(ax == 8){
        writeFile(bx, cx, dx);
    }else if(ax == 9){
        getAllEntries(bx);
    }else if(ax == 10){
        int* resultPtr = (int*)cx; 
        *resultPtr = checkEntryExists(bx);
    }else{
        printString("Invalid interrupt!\0", 1);
    }
}