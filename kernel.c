
int printChar(char character, int color, int line, int col){
    int videoMemoryOrigin = 0xB8000;

    int charPerLine = 80;
    int totalLines = 25;
    int charSize = 2;

    int relativePosition = charPerLine * line + col;
    relativePosition *= charSize;

    int absolutePosition = videoMemoryOrigin + relativePosition;

    int segment = (int)(absolutePosition / 0x1000);
    int remainingAddress = (int)(absolutePosition % 0x1000);

    putInMemory(0xB000, remainingAddress, character);
    putInMemory(0xB000, remainingAddress + 1, color);
}

void main()
{
    int videoMemoryOrigin = 0xB8000;

    int charPerLine = 80;
    int totalLines = 25;
    int charSize = 2;

    int AasciiCode = 0x41;
    int white = 0x7;

    printChar("A", white, 0, 0);
}