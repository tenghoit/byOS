#include "string.h"

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