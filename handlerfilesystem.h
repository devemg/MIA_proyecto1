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

int getBitmapIndex(int startBm,int finBm,char path[]);

void writeSuperBlock(SuperBlock *sb,char path[],int init);

void writeBlockDirectory(BlockDirectory *sb,char path[],int init);

void writeBlockFile(BlockFile *sb,char path[],int init);

void writeBlockPointer(BlockPointer *sb,char path[],int init);

void writeBitmap(int cantInodos,int init,char path[]);

void writeInodo(Inodo*,char[],int);

SuperBlock* readSuperBlock(char[],char[]);

Inodo* readInodo(char[],int);

Inodo* getNewInodoDir();

BlockDirectory* getNewBlockDir(int indexInodo,char name[]);

Response addnewInodo(Inodo* nuevo,int indexI,TypeBlock tipoBloque,BlockDirectory *dir,BlockFile *file,char[],SuperBlock*,char[]);

Response createDirectory(SuperBlock *sb,char path[],char nameDir[]);

void reportTree(char path_report[],char id[]);

Response reportBitmap(int,char[],char[],char[]);

void reportSuperBlock(char[],char[],char[]);

void graphInodo(Inodo*,int,FILE*,int,int,char[]);

void graphBlockDirectory(BlockDirectory*,int initBlock,FILE *myFile,int indexInodo);

void graphAllInodes(SuperBlock *sb,FILE *file_report,char path[]);

void writeConnection(int indexnodo,int indexBloque,int indexPuerto,FILE *myFile);

#endif // HANDLERFILESISTEM_H
