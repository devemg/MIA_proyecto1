#ifndef HANDLERFILESISTEM_H
#define HANDLERFILESISTEM_H
#include <iostream>
#include <structures.h>
#include <enums.h>
#include <string.h>
#include <handlerdisk.h>

Response formatPart(char path[],char partition[],DeleteType tipoFormateo,FileSistem sistem);

Response getStartPartition(MBR*,char[],int*);

void writeSuperBlock(SuperBlock *sb,char path[],int init);

SuperBlock* readSuperBlock(char[],char[]);

void writeBitmap(int cantInodos,int init,char path[]);

Response reportBitmap(int,char[],char[]);

#endif // HANDLERFILESISTEM_H
