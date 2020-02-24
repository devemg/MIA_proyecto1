#ifndef UTILS_H
#define UTILS_H
#include <string.h>
#include <enums.h>
#include <iostream>
#include <structures.h>
#include <math.h>
#include <handlerdisk.h>
#include <handlerpartitions.h>

using namespace std;

void clearArray(char [],int);

void getFullPathDisk(char [],char[],char[]);

void getFullPath(char [],char[],char[]);

int getSize(int,Unit);

void showMessageError(Response);

void fillSpaceWithZeros(char[],int,int);

float getDecimal(float);

const char* showOption(Options);

bool validateOptionCommand(CommandEnum,Option *);

void letsExecCommands(Command*);

int getInt(string);

void fillOptions(Option *it,int *size, int *add,Fit *fit,Unit *unit,TipoParticion *tipo,DeleteType *deltype,
                 char **path,char *id,char *name);
#endif // UTILS_H
