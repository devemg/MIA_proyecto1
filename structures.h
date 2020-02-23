#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <enums.h>
#include <stdio.h>

//info of partitions
struct Partition
{
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    int part_size;
    char part_name[16];

};

//info of disk
struct MBR
{
    int mbr_tamanio;
    char mbr_fecha_creacion[16];
    int mbr_disk_signature;
    char disk_fit;
    Partition particiones[4];

};

//info of extended partitions
//list with pointers to the next EBR
//null = -1

struct EBR
{
    char part_status;
    char part_fit;
    int part_start;
    int part_size;
    int part_next;
    char part_name[16];
};

class virtualBlock{
public:
    int size;
    int start;
    statusBlock status;
    virtualBlock *next;
    virtualBlock();
    virtualBlock(int,int, statusBlock);
};

class Option
{
public:
    Option(Options op){
        option = op;
        next = NULL;
    }

    Options option;
    char *text;
    int num;
    Fit fit;
    Unit unit;
    TipoParticion type;
    DeleteType delType;

    Option *next;
};

class Command {
public:
    CommandEnum cmd;
    Option *opts;
    Command *next;
    Command(CommandEnum c,Option *o){
        cmd = c;
        opts = o;
        next = NULL;
    }
};



#endif // STRUCTURES_H
