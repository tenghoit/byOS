#ifndef SHELL_H
#define SHELL_H

void createFile(char*, char*);
void executeCommand(char*, char[10][80], char*, char*, char*);
void parseCommand(char*, char*, char*, char*);
int getToken(char*, int, char*);
void clearScreen(int);
int checkFileStatus(char*, int);
void printAllEntries(void);
void getHelp(void);

#endif