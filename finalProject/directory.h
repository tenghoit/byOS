#ifndef DIRECTORY_H
#define DIRECTORY_H

void getAllEntries(char*);
void printAllEntries(void);
void getEntryName(char*, char*, int);
void getEntrySectors(int*, char*, int);
int getEntryIndex(char*, char*);
int getFreeEntryIndex(char*);
int checkEntryExists(char*);
void insertEntry(char*, char*, int);
void removeEntry(char*, char*, int);
int getFreeSector(char*);

#endif