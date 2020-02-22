#ifndef UTILS_H
#define UTILS_H
#include <string.h>
#include <enums.h>
#include <iostream>
#include <structures.h>
#include <math.h>

using namespace std;


void clearArray(char [],int);

void getFullPathDisk(char [],char[],char[]);

void getFullPath(char [],char[],char[]);

int getSize(int,Unit);

void showMessageError(Response);

void fillSpaceWithZeros(char[],int,int);

float getDecimal(float);

char* showOption(Options);

#endif // UTILS_H
