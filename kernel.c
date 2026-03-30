void main()
{
    int videoMemoryOrigin = 0xB8000;

    int charPerLine = 80;
    int totalLines = 25;
    int charSize = 2;

    int AasciiCode = 0x41;
    int white = 0x7;

    putInMemory(0xB000, 0x8140, 'A');
    putInMemory(0xB000, 0x8141, 0x7);
}