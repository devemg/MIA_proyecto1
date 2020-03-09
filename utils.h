#ifndef UTILS_H
#define UTILS_H
#include <string.h>
#include <enums.h>
#include <iostream>
#include <structures.h>
#include <math.h>
#include <handlerdisk.h>
#include <handlerpartitions.h>
#include <scanner.h>
#include <parser.h>

using namespace std;
Command* ListCommand(Command *cmd,bool flag);

void writeCommand(string command,bool);

void readExecCommand(bool);

void clearArray(char [],int);

void getFullPathDisk(char [],char[],char[]);

void getFullPath(char [],char[],char[]);

long getSize(int,Unit);

void showMessageError(Response);

void fillSpaceWithZeros(char[],int,int);

float getDecimal(float);

const char* showOption(Options);

bool validateOptionCommand(CommandEnum,Option *);

void letsExecCommands(Command*);

int getInt(string);

void fillOptions(Option *it,int *size, int *add,Fit *fit,Unit *unit,TipoParticion *tipo,DeleteType *deltype,
                 char **path,char *id,char *name,bool *isAdd,bool *isDelete);

Response getContadorDiscos(int *contadorDiscos,char *id);

void myPause();

#endif // UTILS_H
