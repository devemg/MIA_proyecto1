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
              ERROR_PARTITION_NOT_EXIST };
enum DeleteType {Fast,Full};
enum statusBlock {OCUPADO,LIBRE};
enum CommandEnum {mkdisk,rmdisk,fdisk,mount,unmount,rep,exec};
enum Options {Id,Add,Delete,Type,Path,Unitt,Fitt,Size,Name};

#endif // ENUMS_H
