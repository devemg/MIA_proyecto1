#ifndef HANDLERDISK_H
#define HANDLERDISK_H
#include <enums.h>
#include <structures.h>
#include <utils.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include<stdlib.h>
#include<time.h>

#include <iostream>
using namespace std;

void newDisk(int,Fit,Unit,char[],char[]);

void deleteDisk(char[]);

void writeMBR(MBR*,char[],char[]);

void reportMBR(char[],char[]);

MBR* openMBR();

int getSignature();

void getCurrentDate(char []);



#endif // HANDLERDISK_H
