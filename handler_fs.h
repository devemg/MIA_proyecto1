#ifndef HANDLERFILESISTEM_H
#define HANDLERFILESISTEM_H
#include <iostream>
#include <structures.h>
#include <enums.h>
#include <string.h>
#include <handler_disk.h>
#include <sstream>

Response formatPart(char path[],char partition[],TypeFormat tipoFormateo,FileSistem sistem);

Response getStartPartition(MBR*,char[],int*);

int getInitBlock(SuperBlock* sb,int index);

int getInitInode(SuperBlock* sb,int index);

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

Response createDirectory(bool createMk,char id[],char path[]);

int writeDirectory(SuperBlock *sb,char path[],char nameDir[],char namePad[],int indexPad);

int getIndexBlockDir(Inodo *inodoPivote,BlockDirectory *blockDirPivote,char path[],int init);

Response getFreeIndexDirectory(char nameDir[],char path[],SuperBlock *sb,int*,int*,int*,TypeBlock *type);

Response createChildDirectory(char dirPad[],char dirName[],char path[],SuperBlock *sb,int *indexInodoPadre,int *indexBloqueActual);

int findDirectory(char namedir[],char path[],int *indexInodoActual,SuperBlock *sb);

Response createFile(char newPath[],bool createPath,int size,char path[],char namePartition[]);

Response createFile(char newPath[],bool createPath,char pathFile[],char path[],char namePartition[]);

Response createFileWithText(char newPath[],bool createPath,char text[],int size,char path[],char namePartition[]);

Response createChildFile(int size,char *text,char path[],char dirPad[],char name[],SuperBlock *sb,int indexBloqueActual,int indexInodoPadre);

Response catFile(char filePath[],char path[],char partition[]);

int findFile(char filePath[], char path[], char partition[],char **title);

Response findContentFile(char filePath[], char path[], char partition[],char **content,char **title);

Response getContentFile(int indexInodo, char path[],SuperBlock *sb,char **content);

int getFreeIndexFromBlockPointer(int nivel,Inodo *inodo,int idBloque,char path[],SuperBlock *sb,int *freeIndex);

BlockPointer* getNewBlockPointer();

int createPointersInd(int idPointBlock,int *idBloqueActual,SuperBlock *sb,char path[]);

void saveBlockFile(BlockFile *block,SuperBlock *sb,char path[]);

Response addFileBlockPointers(Inodo *inodo,int *indexofInodo,BlockFile *block,SuperBlock *sb,char path[]);

int findDirectoryInPointers(int level,int indexBlock,int *indexInodoActual,char namedir[],SuperBlock *sb,char path[]);

Group* getGroup(char name[],char *contentg);

User* getUser(char usr[],char *contentUsers);

Response addGroup(char *path,char *partition,char grp[]);

Response addUser(char *path,char *partition,char usr[],char pwd[],char grp[]);

User* getUser(char usr[]);

Response ReplaceContentFile(int indexInode,char *content,char path[],char namePart[]);

int countUsers(char contentUsers[]);

int countGroups(char contentGroups[]);

#endif // HANDLERFILESISTEM_H
