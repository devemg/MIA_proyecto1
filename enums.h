#ifndef ENUMS_H
#define ENUMS_H

enum Fit {BestFit='b',FirstFit='f',WorstFit='w'};
enum Unit {KB='k',MB='m',Byte='b'};
enum Status {Activo=1,Inactivo=0};
enum TipoParticion {Primaria='p',Extendida='e',Logica='l'};
enum Response {SUCCESS,
               ERROR_INSUFICIENT_SPACE,
               ERROR_FULL_PARTITION_PRIMARY,
               ERROR_UNHANDLED,
               ERROR_EXISTS_EXTENDED_PARTITION,
               ERROR_NOT_EXIST_EXTENDED_PARTITION,
               ERROR_READING_EBR,
              ERROR_SIZE_MIN,
              ERROR_PARTITION_NOT_EXIST,
              ERROR_PARTITION_EXIST,
              ERROR_PARTITION_MOUNTED,
              ERROR_ID_MALFORMED,
              ERROR_DISK_NOT_EXIST,
              ERROR_PARTITION_NOT_MOUNTED,
              ERROR_DISK_NOT_MOUNTED,
              ERROR_NOT_SPACE_ADD,
              ERROR_PARTITION_MOUNTED_DEL,
              ERROR_DIR_NOT_EXIST};
enum DeleteType {Fast,Full};
enum statusBlock {OCUPADO,LIBRE};
enum CommandEnum {mkdisk,rmdisk,fdisk,mount,unmount,rep,exec};
enum Options {Id,Add,Delete,Type,Path,Unitt,Fitt,Size,Name};
enum FileSistem {ext2 = 2,ext3 = 3};
enum TypeInodo{Dir=0,File=1};
enum TypeBlock{FILE_,DIRECTORY,PIONTERS};
#endif // ENUMS_H
