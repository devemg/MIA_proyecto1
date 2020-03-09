#include "handlerfilesistem.h"

Response getStartPartition(MBR* disco, char name[], int *init){
    int i;
    for(i = 0; i < 4; i++){
        if(disco->particiones[i].part_status == Activo){
            if(strcmp(disco->particiones[i].part_name,name)==0){
                *init = disco->particiones[i].part_start;
                return SUCCESS;
            }
        }
    }
    return ERROR_PARTITION_NOT_EXIST;
}

Response formatPart(char path[], char partition[], DeleteType tipoFormateo, FileSistem sistem){
    MBR *disco = openMBR(path);
    if(disco==NULL){
        return ERROR_DISK_NOT_EXIST;
    }
    double sizeN;
    int cantInodos;
    int cantBloques;
    sizeN= disco->mbr_tamanio-sizeof(SuperBlock);
    if(sistem == ext2){
        sizeN/=4+sizeof(Inodo)+3*sizeof(Block);
    }else if(sistem == ext3){
        sizeN/=4+sizeof(Inodo)+3*sizeof(Block)+sizeof(Journal);
    }
    cantInodos = floor(sizeN);
    cantBloques = cantInodos*3;
    //cout<<"CANTIDAD DE INODOS: "<<cantInodos<<endl;
    //cout<<"CANTIDAD DE BLOQUES: "<<cantBloques<<endl;

    //crear super bloque
    int initPart = 0;
    Response res = getStartPartition(disco,partition,&initPart);
    if(res != SUCCESS){
        return res;
    }
    int sizebmInodos = 10;
    int sizebmBloques = 10;
    int sizeInodos = 10;
    SuperBlock *sb = (SuperBlock*)malloc(sizeof(SuperBlock));
    sb->s_filesystem_type = sistem;
    sb->s_inodes_count = cantInodos;
    sb->s_blocks_count = cantBloques;
    sb->s_free_blocks_count = 0;
    sb->s_free_inodes_count = 0;
    //sb.s_mtime = new myDate(8,3,2020);
    //sb.s_umtime = new myDate(8,3,2020);
    sb->s_mnt_count = 0;
    sb->s_magic = 0xEF53;
    sb->s_inode_size = sizeof(Inodo);
    sb->s_block_size = sizeof(Block);
    sb->s_firts_ino = 0;
    sb->s_first_blo = 0;
    sb->s_bm_inode_start = initPart+sizeof(SuperBlock);
    sb->s_bm_block_start = sb->s_bm_inode_start+sizebmInodos;
    sb->s_inode_start = sb->s_bm_block_start + sizebmBloques;
    sb->s_block_start = sb->s_inode_start + sizeInodos;
    writeSuperBlock(sb,path,initPart);
    delete disco;
    return SUCCESS;
}

void writeSuperBlock(SuperBlock *sb,char path[],int init){
    FILE * myFile;
     myFile = fopen (path,"rb+");
     if (myFile==NULL)
     {
         cout<<"Error al abrir el archivo\n";
         return;
     }
     //escribir MBR en disco
     fseek(myFile, init, SEEK_SET);
     fwrite(sb, sizeof(SuperBlock), 1, myFile);
     //cerrando stream
     fclose (myFile);
}

SuperBlock* readSuperBlock(char path[], char name[]){
    MBR *disco = openMBR(path);
    if(disco==NULL){
        std::cout<<"Error al leer el disco\n";
        return NULL;
    }
    int init;
    Response res = getStartPartition(disco,name,&init);
    if(res!=SUCCESS){
        return NULL;
    }

    FILE *myFile = fopen(path,"rb+");
    if(myFile==NULL){
        cout<<"Error al abrir el archivo \n";
        return NULL;
    }
    SuperBlock *sb = (SuperBlock*)malloc(sizeof(EBR));

    fseek(myFile, init, SEEK_SET);
    fread(sb, sizeof(SuperBlock), 1, myFile);
    fclose(myFile);
    return sb;
}
