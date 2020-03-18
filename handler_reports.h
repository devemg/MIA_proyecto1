#ifndef HANDLER_REPORTS_H
#define HANDLER_REPORTS_H
#include<enums.h>
#include <structures.h>
#include <handlerfilesystem.h>

using namespace std;

void reportTree(char path_report[],char id[]);

Response reportBitmap(int,char[],char[],char[]);

void reportSuperBlock(char[],char[],char[]);

Response reportInodes(char path[], char name[], char path_report[]);

Response reportBlocks(char path[], char name[], char path_report[]);

Response reportFile(char pathFile[],char path[],char partition[],char reportPath[]);

void graphInodo(Inodo*,int,FILE*,char[],SuperBlock*);

void graphBlockDirectory(BlockDirectory *block,int initBlock, FILE *myFile,int indexInodo,SuperBlock *sb,char path[]);

void graphConnectionInodoBloque(int indexnodo,int indexBloque,int indexPuerto,FILE *myFile);

void graphConnectionBloqueInodo(int indexnodo,int indexBloque,int indexPuerto,FILE *myFile);

void graphBlockFile(BlockFile *block,int initBlock, FILE *myFile,int indexInodo);

Response graphFile(char *text,char *title,char reportPath[]);

void graphBlockPointer(int level,int indexBlock,int indexInodo,FILE *fileReport,char path[],SuperBlock *sb,TypeInode type);

#endif // HANDLER_REPORTS_H
