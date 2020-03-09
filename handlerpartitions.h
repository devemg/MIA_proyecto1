#ifndef HANDLERPARTITIONS_H
#define HANDLERPARTITIONS_H
#include <enums.h>
#include <utils.h>
#include <handlerdisk.h>
#include <virtual_blocks.h>
#include <vars.h>
#include <handlerfilesystem.h>

Response updatepartition(char path[],char nameDisk[],char namePart[],int sizeToMod,Unit unit);

Response modPartition(int size, Unit unit, char path[], char name[]);

Response createPartition(int,Unit,char[],char[],TipoParticion,Fit,char[]);

Response newPartition(int,Unit,char[],TipoParticion,Fit,char[]);

Response newPrimaryPart(long,Fit,char[],MBR*,char[]);

Response newExtendedPart(long,Fit,char[],MBR*,char[]);

Response newLogicPart(long,Fit,char[],MBR*,char[]);

void newEBR(Partition *, char[]);

void writeEBR(EBR *, char[],int);

Response deletePartition(char[], char[],char[],TipoParticion,DeleteType);

Response rmPartition(char[],char[],TipoParticion,DeleteType);

Response deletePrimaryPart(MBR*,char[],DeleteType,char[]);

Response deleteLogicPart(char[], MBR*, char[]);

Response getStartAddress(MBR*,Fit,long,int*);

Response mountPart(char[],char[]);

void showMounts();

char* getPartId(char,int);

Response unmountPart(char[]);

Response getStartAddressLogic(MBR *disco,EBR *part,Fit fit,long size,int *startPoint,char[]);

#endif // HANDLERPARTITIONS_H
