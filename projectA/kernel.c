void printWelcome(void);

void clearScreen(void);

void printAt(int offset, char c, char color);

void main()
{
    
    clearScreen();
    printWelcome();

    while(1);
}


void printWelcome(){
    char color = 0x4F;
    int start = 1822;
    int startHex = 0x71E;

    /*
    printAt(start + 0, 'W', color);
    printAt(start + 2, 'e', color);
    printAt(start + 4, 'l', color);
    printAt(start + 6, 'c', color);
    printAt(start + 8, 'o', color);
    printAt(start + 10, 'm', color);
    printAt(start + 12, 'e', color);
    printAt(start + 14, ' ', color);
    printAt(start + 16, 't', color);
    printAt(start + 18, 'o', color);
    printAt(start + 20, ' ', color);
    printAt(start + 22, 'm', color);
    printAt(start + 24, 'y', color);
    printAt(start + 26, ' ', color);
    printAt(start + 28, 'O', color);
    printAt(start + 30, 'S', color);
    printAt(start + 32, '!', color);
    */




putInMemory(0xB000, 0x87B6, 'W'); 
    putInMemory(0xB000, 0x87B7, 0x4F);
    /* e */
    putInMemory(0xB000, 0x87B8, 'e');
    putInMemory(0xB000, 0x87B9, 0x4F);
    /* l */
    putInMemory(0xB000, 0x87BA, 'l');
    putInMemory(0xB000, 0x87BB, 0x4F);
    /* c */
    putInMemory(0xB000, 0x87BC, 'c');
    putInMemory(0xB000, 0x87BD, 0x4F);
    /* o */
    putInMemory(0xB000, 0x87BE, 'o');
    putInMemory(0xB000, 0x87BF, 0x4F);
    /* m */
    putInMemory(0xB000, 0x87C0, 'm');
    putInMemory(0xB000, 0x87C1, 0x4F);
    /* e */
    putInMemory(0xB000, 0x87C2, 'e');
    putInMemory(0xB000, 0x87C3, 0x4F);

    /* (Space) */
    putInMemory(0xB000, 0x87C4, ' ');
    putInMemory(0xB000, 0x87C5, 0x4F);

    /* t */
    putInMemory(0xB000, 0x87C6, 't');
    putInMemory(0xB000, 0x87C7, 0x4F);
    /* o */
    putInMemory(0xB000, 0x87C8, 'o');
    putInMemory(0xB000, 0x87C9, 0x4F);

    /* (Space) */
    putInMemory(0xB000, 0x87CA, ' ');
    putInMemory(0xB000, 0x87CB, 0x4F);

    /* m */
    putInMemory(0xB000, 0x87CC, 'm');
    putInMemory(0xB000, 0x87CD, 0x4F);
    /* y */
    putInMemory(0xB000, 0x87CE, 'y');
    putInMemory(0xB000, 0x87CF, 0x4F);

    /* (Space) */
    putInMemory(0xB000, 0x87D0, ' ');
    putInMemory(0xB000, 0x87D1, 0x4F);

    /* O */
    putInMemory(0xB000, 0x87D2, 'O');
    putInMemory(0xB000, 0x87D3, 0x4F);
    /* S */
    putInMemory(0xB000, 0x87D4, 'S');
    putInMemory(0xB000, 0x87D5, 0x4F);
    /* ! */
    putInMemory(0xB000, 0x87D6, '!');
    putInMemory(0xB000, 0x87D7, 0x4F);

}

void clearScreen(){ 
    int curr;

    int wholeScreen = 0x8FA0;
    int tillBooting = 0x8640;

    for(curr = 0x0; curr < 0x0FA0; curr += 2){
        printAt(curr, 0x20, 0x7);
    }
}


void printAt(int offset, char c, char color){
    putInMemory(0xB000, 0x8000 + offset, c);
    putInMemory(0xB000, 0x8001 + offset, color);
}