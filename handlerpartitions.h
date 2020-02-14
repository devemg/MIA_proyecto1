#ifndef HANDLERPARTITIONS_H
#define HANDLERPARTITIONS_H
#include <enums.h>
#include <utils.h>
#include <handlerdisk.h>

void createPartition(int,Unit,char[],char[],TipoParticion,Fit,char[]);

Response newPartition(int,Unit,char[],TipoParticion,Fit,char[]);

Response newPrimaryPart(int,Fit,char[],MBR*,char[]);

Response newExtendedPart(int,Fit,char[],MBR*,char[]);

Response newLogicPart(int,Fit,char[],MBR*,char[]);

void newEBR(Partition *, char[]);

void writeEBR(EBR *, char[],int);

#endif // HANDLERPARTITIONS_H
