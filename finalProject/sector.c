#include "sector.h"
#include "math.h"

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