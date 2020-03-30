#ifndef UTILS_H
#define UTILS_H
#include <string.h>
#include <enums.h>
#include <iostream>
#include <structures.h>
#include <math.h>
#include <handler_disk.h>
#include <handler_partitions.h>
#include <scanner.h>
#include <parser.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <dirent.h>

using namespace std;

void clearArray(char [],int);

string getNamePath(char *path,int *poss);

string getPathWithoutName(char *path,int sizeName);

void getFullPathDisk(char [],char[],char[]);

void getFullPath(char [],char[],char[]);

long getSize(int,Unit);

void showMessageError(Response);

void fillSpaceWithZeros(char[],int,int);

float getDecimal(float);

const char* showOption(Options);

int getInt(string);

Response getContadorDiscos(int *contadorDiscos,char *id);

void createPath(string path);

#endif // UTILS_H
