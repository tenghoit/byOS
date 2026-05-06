#include "file.h"
#include "directory.h"
#include "sector.h"
#include "string.h"
#include "math.h"


void readFile(char* fileName, char* buffer){

    char dirSector[512];
    int entrySectors[26];
    int targetIndex;

    readSector(dirSector, 2);

    targetIndex = getEntryIndex(dirSector, fileName);
    if(targetIndex == -1){
        interrupt(0x21, 0, "File not found: ", 0, 0);
        interrupt(0x21, 0, fileName, 1, 0);
        return;
    }

    getEntrySectors(entrySectors, dirSector, targetIndex);
    loadFile(buffer, entrySectors);
}

void writeFile(char* fileName, char* buffer){
    int fileEntrySize = 32;
    int fileNameSize = 6;
    int fileSectors = 26;

    char dirSector[512];
    char mapSector[512];
    char entryNames[16][7];
    int entrySectors[26];
    int targetIndex;
    int i;
    int freeSector;
    int numberOfSectors = div(getStringLength(buffer), 512) + 1;
    int result;

    readSector(mapSector, 1);
    readSector(dirSector, 2);  

    targetIndex = getEntryIndex(dirSector, fileName);
    if(targetIndex != -1){
        interrupt(0x21, 0, "File already exists: ", 0, 0);
        interrupt(0x21, 0, fileName, 1, 0);
        return;
    }

    targetIndex = getFreeEntryIndex(dirSector);
    if(targetIndex == -1){
        interrupt(0x21, 0, "Directory full", 0, 0);
        return;
    }
    insertEntry(dirSector, fileName, targetIndex);

    for(i = 0; i < numberOfSectors; i++){

        freeSector = getFreeSector(mapSector);
        if(freeSector == -1){
            interrupt(0x21, 0, "no free sectors", 1, 0);
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
        interrupt(0x21, 0, "could not find ", 0, 0);
        interrupt(0x21, 0, fileName, 1, 0);
        return;
    }

    removeEntry(dirSector, mapSector, targetIndex);

    interrupt(0x21, 6, mapSector, 1, 0);
    interrupt(0x21, 6, dirSector, 2, 0);
}

void loadFile(char* buffer, int* entrySectors){
    int fileSectors = 26;
    int i;
    for(i = 0; i < fileSectors; i++){
        if(entrySectors[i] == 0x0){
            buffer[i * 512] = 0x0;
            break;
        }

        readSector(buffer + (i * 512), entrySectors[i]);
    }
    
}