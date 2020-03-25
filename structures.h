#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <enums.h>
#include <iostream>

//info of partitions
struct Partition
{
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    long part_size;
    char part_name[16];

};

//info of disk
struct MBR
{
    long mbr_tamanio;
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
    long part_size;
    int part_next;
    char part_name[16];
};

class virtualBlock{
public:
    long size;
    int start;
    statusBlock status;
    virtualBlock *next;
    virtualBlock();
    virtualBlock(int,int, statusBlock);
};


class MountedPart{
public:
    char name[75];
    char id[10];
    int start;
};



class MountedDisk{
  public:
    char path[100];
    char letter;
    MountedPart *parts[60];

    MountedDisk(){
        int i;
        for(i=0;i<60;i++)
            parts[i]=NULL;
    }
};

//FASE 2 **************************************************************

struct SuperBlock{
    int s_filesystem_type;
    int s_inodes_count;
    int s_blocks_count;
    int s_free_blocks_count;
    int s_free_inodes_count;
    char s_mtime[16];
    char s_umtime[16];
    int s_mnt_count;
    int s_magic;
    int s_inode_size;
    int s_block_size;
    int s_firts_ino;
    int s_first_blo;
    int s_bm_inode_start;
    int s_bm_block_start;
    int s_inode_start;
    int s_block_start;
};

struct Inodo{
    int i_uid;
    int i_gid;
    int i_size;
    char i_atime[16];
    char i_ctime[16];
    char i_mtime[16];
    int i_block[15];
    TypeInode i_type;
    int i_perm;
};

struct Content{
    char b_name[12];
    int b_inodo;
};

struct BlockDirectory
{
    Content b_content[4];
};

struct BlockFile
{
    char b_content[64];
};

struct BlockPointer
{
    int b_pointers[16];
};


struct Journal{

};

class Sesion{
public:
    char *user;
    char *path;
    char *namePartition;
    char *id;
    Sesion(){
        this->user = NULL;
        this->path = NULL;
        this->namePartition = NULL;
        this->id = NULL;
    }

    bool isActive(){
        return user!=NULL && path!=NULL && namePartition!=NULL && id!=NULL;
    }

    void clear(){
        user = NULL;
        path = NULL;
        namePartition = NULL;
        id = NULL;
    }
};


class User{
public:
    std::string id;
    char type;
    std::string name;
    std::string pwd;
    std::string group;
};

class Group{
public:
    std::string id;
    std::string name;
};

#endif // STRUCTURES_H
