#include "math.h"
#include "string.h"
#include "sector.h"
#include "directory.h"
#include "file.h"

void terminate(void);
void executeProgram(char*, int);
void printString(char*, int);
void printChar(char);
void readString(char*);

void makeInterrupt21(void);

void main(){

    makeInterrupt21();
    
    interrupt(0x21, 5, 0, 0, 0);
}

void terminate(){
    interrupt(0x21, 4, "cli", 0x2000, 0);
}

void executeProgram(char* name, int segment){
    char buffer[13312];
    int i;

    if(checkEntryExists(name) == 0){
        interrupt(0x21, 0, "File not found: ", 0, 0);
        interrupt(0x21, 0, name, 1, 0);
        return;
    }

    readFile(name, buffer);
  
    for(i = 0; i < 13312; i++){
        putInMemory(segment, i, buffer[i]);
    }

    interrupt(0x21, 0, "executing ", 0, 0);
    interrupt(0x21, 0, name, 1, 0);

    launchProgram(segment);
}

void printString(char* str, int newLine){

    int index = 0;
    while(str[index] != 0x0){    
        printChar(str[index]);
        index++;
    }

    if(newLine == 1){
        interrupt(0x10,0xe*256+0xd, 0, 0, 0);
        interrupt(0x10,0xe*256+0xa, 0, 0, 0);
    }else if(newLine == 2){
        interrupt(0x10, 0xe * 256 + 0xd, 0, 0, 0); 
    }
}

void printChar(char c){
    char ah = 0xe;
    int ax = ah * 256 + c;
    interrupt(0x10, ax, 0, 0, 0);
}

void readString(char* str){
    int index = 0;
    char enterKey = 0xd;
    char backspaceKey = 0x8;
    char spaceKey = 0x20;
    char c;

    while(1){

        c = interrupt(0x16, 0, 0, 0, 0);

        if (c == enterKey){

            str[index] = 0x0;
            printChar(0xd);
            printChar(0xa);
            break;

        }else if(c == backspaceKey){

            if(index <= 0) continue;

            printChar(backspaceKey);
            printChar(spaceKey);
            printChar(backspaceKey);

            index--;

        }else{

            printChar(c);
            str[index] = c;
            index++;

        }
    }    
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
        writeFile(bx, cx);
    }else if(ax == 9){
        getAllEntries(bx);
    }else if(ax == 10){
        int* resultPtr = (int*)cx; 
        *resultPtr = checkEntryExists(bx);
    }else{
        printString("Invalid interrupt", 1);
    }
}