#ifndef HANDLERPARTITIONS_H
#define HANDLERPARTITIONS_H
#include <enums.h>
#include <utils.h>
#include <handlerdisk.h>

void createPartition(int,Unit,char[],char[],TipoParticion,Fit,char[]);

void newPartition(int,Unit,char[],TipoParticion,Fit,char[]);

void newPrimaryPart(int,Fit,char[],MBR*);



#endif // HANDLERPARTITIONS_H
