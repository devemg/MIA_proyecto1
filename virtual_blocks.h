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


class virtualBlock_tracker {
virtualBlock *start;
public:
void addVirtualBlock(int,int,statusBlock);
void showTracker();
virtualBlock_tracker();
int getNewPoint(Fit,int);
};
#endif // VIRTUAL_SPACE_H
