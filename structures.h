#ifndef STRUCTURES_H
#define STRUCTURES_H

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


#endif // STRUCTURES_H
