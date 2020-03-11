#ifndef HANDLERFILESISTEM_H
#define HANDLERFILESISTEM_H
#include <iostream>
#include <structures.h>
#include <enums.h>
#include <string.h>
#include <handlerdisk.h>
#include <sstream>

Response formatPart(char path[],char partition[],DeleteType tipoFormateo,FileSistem sistem);

Response getStartPartition(MBR*,char[],int*);

void writeSuperBlock(SuperBlock *sb,char path[],int init);

SuperBlock* readSuperBlock(char[],char[]);

void writeBitmap(int cantInodos,int init,char path[]);

Response reportBitmap(int,char[],char[],char[]);

void reportSuperBlock(char[],char[],char[]);

Response createDir(bool createMk,char id[],char path[]);

Inodo* readInodo(char[],int);

void writeInodo(Inodo*,char[],int);

Inodo* getNewInodoDir();

void reportTree(char path_report[],char id[]);

void graphInodo(Inodo*,FILE*);

#endif // HANDLERFILESISTEM_H
