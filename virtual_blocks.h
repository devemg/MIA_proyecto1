#ifndef VIRTUAL_SPACE_H
#define VIRTUAL_SPACE_H
#include <enums.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <handlerdisk.h>

#include <enums.h>
#include <iostream>

class virtualBlock{
public:
    int size;
    int start;
    statusBlock status;
    virtualBlock *next;
    virtualBlock();
    virtualBlock(int,int, statusBlock);
};


class virtualBlock_tracker {
virtualBlock *start;
public:
void addVirtualBlock(int,int,statusBlock);
void showTracker();
virtualBlock_tracker();
int getNewPoint(Fit,int);
};

int getStartPoint(int,MBR*,Fit);

#endif // VIRTUAL_SPACE_H
