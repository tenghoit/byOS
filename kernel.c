
int printChar(char character, int color, int line, int col){
    int videoMemoryOrigin = 0xB8000;

    int charPerLine = 80;
    int totalLines = 25;
    int charSize = 2;

    int offset = (charPerLine * line + col) * charSize;

    /*
    int absolutePosition = videoMemoryOrigin + offset;
    int remainingAddress = (int)(absolutePosition % 0x1000);
    */ 

    int segment = 0xB000;
    int remaining = 0x8000;

    putInMemory(0xB000, remaining, character);
    putInMemory(0xB000, remaining+1, color);
}

void printHelloWorld(){
    putInMemory(0xB000, 0x80A0, 'H');
    putInMemory(0xB000, 0x80A1, 0x7);
   
    putInMemory(0xB000, 0x80A2, 'E');
    putInMemory(0xB000, 0x80A3, 0x7);

    putInMemory(0xB000, 0x80A4, 'L');
    putInMemory(0xB000, 0x80A5, 0x7);

    putInMemory(0xB000, 0x80A6, 'L');
    putInMemory(0xB000, 0x80A7, 0x7);

    putInMemory(0xB000, 0x80A8, 'O');
    putInMemory(0xB000, 0x80A9, 0x7);

 

    putInMemory(0xB000, 0x80AA, 'W');
    putInMemory(0xB000, 0x80AB, 0x7);

    putInMemory(0xB000, 0x80AC, 'O');
    putInMemory(0xB000, 0x80AD, 0x7);

    putInMemory(0xB000, 0x80AE, 'R');
    putInMemory(0xB000, 0x80AF, 0x7);

    putInMemory(0xB000, 0x80B0, 'L');
    putInMemory(0xB000, 0x80B1, 0x7);

    putInMemory(0xB000, 0x80B2, 'D');
    putInMemory(0xB000, 0x80B3, 0x7);

}

void main()
{
    int AasciiCode = 0x41;
    int white = 0x7;
     
 
    putInMemory(0xB000, 0x8140, 'A');
    putInMemory(0xB000, 0x8141, 0x7);

}