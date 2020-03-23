#ifndef ENUMS_H
#define ENUMS_H

enum Fit {BestFit='b',FirstFit='f',WorstFit='w',FIT_ERROR};
enum Unit {KB='k',MB='m',Byte='b',UNIT_ERROR};
enum Status {Activo=1,Inactivo=0};
enum TypePartition {Primaria='p',Extendida='e',Logica='l',TP_ERROR};
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
              ERROR_DIR_NOT_EXIST,
              ERROR_INODO_NOT_DIR,
              ERROR_LEVEL_FULL};
enum TypeFormat {Fast,Full,TF_ERROR};
enum statusBlock {OCUPADO,LIBRE};
enum CommandEnum {mkdisk,
                  rmdisk,
                  fdisk,
                  mount,
                  unmount,
                  rep,
                  exec,
                 loss,
                  chgrp,
                  ch_own,
                  find_,
                  mv,
                  cp,
                  mk_dir,
                  ren,
                 edit,
                  rem,
                  cat,
                  mkfile,
                  ch_mod,
                  rmusr,
                  mkusr,
                 login,
                  mkfs,
                  mkgrp,
                  rmgrp};
enum Options {Id,
              Add,
              Delete,
              Type,
              Path,
              Unitt,
              Fitt,
              Size,
              Name,
              Format,
              FileSys,
              Ruta,
              Dest,
              Fil_e,
              Cont,
              Recursive,
              Ugo,
              Grp,
              Pwd,
              Usr};
enum FileSistem {ext2 = 2,ext3 = 3};
enum TypeInode{IN_FILE = 0,IN_DIRECTORY=1};
enum TypeBlock{FILE_,DIRECTORY,POINTERS};
enum TypeReport{Inode,Journaling,Block,BM_inode,BM_block,Tree,Sb,File,Ls,Mbr,Disk};
#endif // ENUMS_H
