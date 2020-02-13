#ifndef STRUCTURES_H
#define STRUCTURES_H

//info of partitions
typedef struct
{
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    int part_size;
    char part_name[16];

}Partition;

//info of disk
typedef struct
{
    int mbr_tamanio;
    char mbr_fecha_creacion[16];
    int mbr_disk_signature;
    char disk_fit;
    Partition particiones[4];

}MBR;

//info of extended partitions
//list with pointers to the next EBR
//null = -1

typedef struct
{
    char part_status;
    char part_fit;
    int part_start;
    int part_size;
    int part_next;
    char part_name[16];
}EBR;


#endif // STRUCTURES_H
