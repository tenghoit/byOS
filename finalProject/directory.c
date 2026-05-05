#include "directory.h"
#include "sector.h"
#include "string.h"


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
    int j;

    for(i = 0; i < fileNameSize; i++){
        if(fileName[i] == 0x0 || nameFinished == 1){
            dirSector[offset + i] = 0x00;
            nameFinished = 1;
        } else {
            dirSector[offset + i] = fileName[i];
        }
    }

    for(j = offset + fileNameSize; j < offset + fileEntrySize; j++){
        dirSector[j] = 0x00;
    }
}

void removeEntry(char* dirSector, char* mapSector, int index){
    int fileEntrySize = 32;
    int fileSectors = 26;
    int fileNameSize = 6;
    int start = index * fileEntrySize + fileNameSize;
    int end = start + fileSectors;
    int i;

    dirSector[index * fileEntrySize] = 0x00;

    for(i = start; i < end; i++){
        if(dirSector[i] == 0x0){
            break;
        }
        mapSector[dirSector[i] + 1] = 0x00;
        dirSector[i] = 0x0;
    }

    interrupt(0x21, 0, "cleared entry\0", 1, 0);
}

int getFreeSector(char* mapSector){
    int i;
    for(i = 3; i < 512; i++){
        if(mapSector[i] == 0x00){
            return i;
        }
    }
    return -1;
}