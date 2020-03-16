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

BlockDirectory* readBlockDirectory(char path[], int init);

SuperBlock* readSuperBlock(char[],char[],int*);

SuperBlock* readSuperBlock(char[],char[]);

Inodo* readInodo(char[],int);

BlockPointer* readBlockPointer(char path[], int init);

BlockFile* readBlockFile(char path[], int init);

Inodo* getNewInodo(TypeInode type,int permisos,int size);

BlockDirectory* getNewBlockDir(char name[],int indexDir,char namepad[],int indexPad);

BlockFile* getNewBlockFile();

Response addnewInodo(Inodo* nuevo,int indexI,TypeBlock tipoBloque,BlockDirectory *dir,BlockFile *file,char[],SuperBlock*,char[]);

Response createDirectory(bool createMk,char id[],char path[]);

int writeDirectory(SuperBlock *sb,char path[],char nameDir[],char namePad[],int indexPad);

void reportTree(char path_report[],char id[]);

Response reportBitmap(int,char[],char[],char[]);

void reportSuperBlock(char[],char[],char[]);

void graphInodo(Inodo*,int,FILE*,char[],SuperBlock*);

void graphBlockDirectory(BlockDirectory *block,int initBlock, FILE *myFile,int indexInodo,SuperBlock *sb,char path[]);

void graphConnectionInodoBloque(int indexnodo,int indexBloque,int indexPuerto,FILE *myFile);

void graphConnectionBloqueInodo(int indexnodo,int indexBloque,int indexPuerto,FILE *myFile);

void graphBlockFile(BlockFile *block,int initBlock, FILE *myFile,int indexInodo);

int getIndexBlockDir(Inodo *inodoPivote,BlockDirectory *blockDirPivote,char path[],int init);

Response getFreeIndexDirectory(char nameDir[],char path[],SuperBlock *sb,int*,int*,int*);

Response createChildDirectory(char dirPad[],char dirName[],char path[],SuperBlock *sb,int startSb,int *indexInodoPadre,int *indexBloqueActual);

int findDirectory(char namedir[],char path[],int *indexInodoActual,SuperBlock *sb);

Response createFile(char newPath[],bool createPath,int size,char path[],char namePartition[]);

Response createFile(char newPath[],bool createPath,char pathFile[],char path[],char namePartition[]);

int createChildFile(int size,char *text,char path[],SuperBlock *sb);

Response reportInodes(char path[], char name[], char path_report[]);

Response reportBlocks(char path[], char name[], char path_report[]);

#endif // HANDLERFILESISTEM_H
