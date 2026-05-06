#include "string.h"

int stringEquals(char* str1, char* str2){
    int index = 0;
    int i;
    int length;

    length = getStringLength(str1);
    if (length != getStringLength(str2)){
        return 0;
    }

    for(i = 0; i < length; i++){
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

void clearString(char* str, int length){
    int i;
    for(i = 0; i < length; i++){
        str[i] = 0x0;
    }
}

void copyString(char* str1, char* str2){
    char c;
    int i = 0;
    while (1)
    {
        c = str1[i];
        str2[i] = c;
        if(c == 0x0){
            break;
        }
        i++;
    }
    
}