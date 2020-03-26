#include "handler_fs.h"

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

int getBitmapIndex(int startBm,int finBm,char path[]){
    FILE *myFile = fopen(path,"rb+");
    if(myFile==NULL){
        cout<<"Error al abrir el disco \n";
        return -1;
    }
    char caracter;
    fseek(myFile, startBm, SEEK_SET);
    int contador = 0;
    bool marcado = false;
    while (contador<finBm) {
        fread(&caracter,sizeof(char),1,myFile);
        if(caracter == '0'){
            if(!marcado){
                fseek(myFile, -1, SEEK_CUR);
                fwrite("1",sizeof(char),1,myFile);
                marcado = true;
            }else{
                break;
            }
        }
        contador++;
    }
    fclose(myFile);
    return contador;
}

Response formatPart(char path[], char partition[], TypeFormat tipoFormateo, FileSistem sistem){
    MBR *disco = openMBR(path);
    if(disco==NULL){
        return ERROR_DISK_NOT_EXIST;
    }
    double sizeN;
    int cantInodos;
    int cantBloques;
    sizeN= disco->mbr_tamanio-sizeof(SuperBlock);
    if(sistem == ext2){
        sizeN/=4+sizeof(Inodo)+3*sizeof(BlockFile);
    }else if(sistem == ext3){
        sizeN/=4+sizeof(Inodo)+3*sizeof(BlockFile)+sizeof(Journal);
    }
    cantInodos = floor(sizeN);
    cantBloques = cantInodos*3;
    //crear super bloque
    int initPart = 0;
    Response res = getStartPartition(disco,partition,&initPart);
    if(res != SUCCESS){
        return res;
    }
    int sizeInodos = sizeof(Inodo)*cantInodos;
    //BLOQUES
    SuperBlock *sb = (SuperBlock*)malloc(sizeof(SuperBlock));
    sb->s_filesystem_type = sistem;
    getCurrentDate(sb->s_mtime);
    getCurrentDate(sb->s_umtime);
    sb->s_mnt_count = 0;
    sb->s_magic = 0xEF53;
    sb->s_inode_size = sizeof(Inodo);
    sb->s_block_size = sizeof(BlockFile);
    sb->s_firts_ino = 0;
    sb->s_first_blo = 0;
    if(sistem == ext2){
        sb->s_bm_inode_start = initPart+sizeof(SuperBlock);
    }else{
        sb->s_bm_inode_start = initPart+sizeof(SuperBlock)+sizeof(Journal);
    }
    sb->s_bm_block_start = sb->s_bm_inode_start+cantInodos;
    sb->s_inode_start = sb->s_bm_block_start + cantBloques;
    sb->s_block_start = sb->s_inode_start + sizeInodos;
    sb->s_free_blocks_count = cantBloques;
    sb->s_free_inodes_count = cantInodos;
    sb->s_blocks_count = cantBloques;
    sb->s_inodes_count = cantInodos;
    writeSuperBlock(sb,path,initPart);
    //BITMAP DE INODOS
    writeBitmap(cantInodos,sb->s_bm_inode_start,path);
    //BITMAP DE BLOQUES
    writeBitmap(cantBloques,sb->s_bm_block_start,path);
    //CREAR CARPETA RAIZ
    writeDirectory(sb,path,"/","/",0);
    writeSuperBlock(sb,path,initPart);
    //CREAR ARCHIVO DE USERS
    char *users = "1,G,root\n1,U,root,root,123\n";
    createFileWithText("/users.txt",true,users,28,path,partition);
    delete disco;
    delete sb;
    return SUCCESS;
}

Response createDirectory(bool createMk,char id[],char path[]){
    MountedDisk *disk =getMountedDisk(id);
    if(disk==NULL){
        return ERROR_UNHANDLED;
    }

    MountedPart *partition =getMountedPartition(id);
    if(partition==NULL){
        return ERROR_UNHANDLED;
    }

    //VALIDAR QUE HAYA ESPACIO PARA CREAR INODOS Y BLOQUES
    int startSb;
    SuperBlock *sb = readSuperBlock(disk->path,partition->name,&startSb);
    if(sb==NULL){
        return ERROR_UNHANDLED;
    }
    int indexInodoPadre = 0;
    int indexBloqueActual = 0;
    std::stringstream ss(path);
        std::string token;
        std::string dirPad="/";
        while (std::getline(ss, token, '/')) {
            if(token!=""){
                //cout<<"padre: "<<dirPad<<endl;
                //cout<<"carpeta: "<<token<<endl;
                if (ss.tellg() == -1) {
                    Response res = createChildDirectory(&dirPad[0],&token[0],disk->path,sb,&indexInodoPadre,&indexBloqueActual);
                    if(res!=SUCCESS)return res;
                }else{
                    int indexBloque = findDirectory(&token[0],disk->path,&indexInodoPadre,sb);
                    if(indexBloque!=-1){
                        indexBloqueActual = indexBloque;
                    }else{
                        if(createMk){
                            Response res = createChildDirectory(&dirPad[0],&token[0],disk->path,sb,&indexInodoPadre,&indexBloqueActual);;
                            if(res!=SUCCESS){
                                return res;
                            }
                        }else{
                            return ERROR_DIR_NOT_EXIST;
                        }
                    }

                }
                dirPad = token;
            }
        }
   writeSuperBlock(sb,disk->path,startSb);
 return SUCCESS;
}

int findDirectory(char namedir[],char path[],int *indexInodoActual,SuperBlock *sb){
    Inodo *inodo = readInodo(path,getInitInode(sb,*indexInodoActual));
    if(inodo==NULL){
        return -1;
    }
    int indexBlock;
    BlockDirectory *block;
    for(indexBlock = 0;indexBlock<12;indexBlock++){
        if(inodo->i_block[indexBlock]!=-1){
            if(inodo->i_type == IN_DIRECTORY){
                block = readBlockDirectory(path,getInitBlock(sb,inodo->i_block[indexBlock]));
                if(block == NULL){
                    return -1;
                }
                int i;
                for(i=2;i<4;i++){
                    if(block->b_content[i].b_inodo!=-1){
                        if(strcmp(block->b_content[i].b_name,namedir)==0){
                            *indexInodoActual = block->b_content[i].b_inodo;
                            inodo = readInodo(path,getInitInode(sb,*indexInodoActual));
                            if(inodo==NULL){
                                return -1;
                            }
                            return inodo->i_block[0];
                        }
                    }
                }
            }
        }
    }
    for(indexBlock = 12;indexBlock<14;indexBlock++){
        int index = findDirectoryInPointers(indexBlock-11,inodo->i_block[indexBlock],indexInodoActual,namedir,sb,path);
        if(index!=-1)return index;
    }
    return -1;
}

int findDirectoryInPointers(int level,int indexBlock,int *indexInodoActual,char namedir[],SuperBlock *sb,char path[]){

    if(level == 1){
        BlockPointer *pointers = readBlockPointer(path,getInitBlock(sb,indexBlock));
        int indexofPointer;
        for(indexofPointer = 0;indexofPointer<14;indexofPointer++){
            if(pointers->b_pointers[indexofPointer]!=-1){
               //BUSCANDO EN INODO
                Inodo *inodo = readInodo(path,getInitInode(sb,pointers->b_pointers[indexofPointer]));
                if(inodo==NULL)return -1;
                int indexBlock;
                BlockDirectory *block;
                for(indexBlock = 0;indexBlock<12;indexBlock++){
                    if(inodo->i_block[indexBlock]!=-1){
                        if(inodo->i_type == IN_DIRECTORY){
                            block = readBlockDirectory(path,getInitBlock(sb,inodo->i_block[indexBlock]));
                            if(block == NULL)return -1;
                            int i=0;
                                if(block->b_content[i].b_inodo!=-1){
                                    if(strcmp(block->b_content[i].b_name,namedir)==0){
                                        *indexInodoActual = block->b_content[0].b_inodo;
                                        return inodo->i_block[indexBlock];
                                    }
                                }
                        }
                    }
                }
            }
        }

    }else{
        BlockPointer *pointers = readBlockPointer(path,getInitBlock(sb,indexBlock));
        int indexofPointer;
        for(indexofPointer = 0;indexofPointer<14;indexofPointer++){
            if(pointers->b_pointers[indexofPointer]!=-1){
                int i = findDirectoryInPointers(level-1,pointers->b_pointers[indexofPointer],indexInodoActual,namedir,sb,path);
                if(i!=-1){
                    return i;
                }
            }
        }

    }

    return -1;
}

Response createChildDirectory(char dirPad[],char dirName[],char path[],SuperBlock *sb,int *indexInodoPadre,int *indexBloqueActual){
    int indexFree = -1;
    TypeBlock type = DIRECTORY;
    Response res = getFreeIndexDirectory(dirPad,path,sb,indexBloqueActual,indexInodoPadre,&indexFree,&type);
    if(res!=SUCCESS){
        return res;
    }
    if(indexFree == -1){
        return ERROR_LEVEL_FULL;
    }
    int indexnew = writeDirectory(sb,path,dirName,dirPad,*indexInodoPadre);
    if(type == DIRECTORY){
        BlockDirectory *block = readBlockDirectory(path,getInitBlock(sb,*indexBloqueActual));
        block->b_content[indexFree].b_inodo = indexnew;
        strcpy(block->b_content[indexFree].b_name,dirName);
        writeBlockDirectory(block,path,getInitBlock(sb,*indexBloqueActual));
        delete block;
    }else if(type == POINTERS){
        BlockPointer *block = readBlockPointer(path,getInitBlock(sb,*indexBloqueActual));
        block->b_pointers[indexFree] = indexnew;
        writeBlockPointer(block,path,getInitBlock(sb,*indexBloqueActual));
       // delete block;
    }
    *indexInodoPadre = indexnew;

    return SUCCESS;
}

Response getFreeIndexDirectory(char nameDir[],char path[],SuperBlock *sb,int *indexBloqueActual,int *indexInodoActual,int *indexFree,TypeBlock *type){
    Inodo *inodo = NULL;
    inodo = readInodo(path,getInitInode(sb,*indexInodoActual));
    if(inodo==NULL){
        return ERROR_DIR_NOT_EXIST;
    }
    int idPointBlock=0;
    bool isDirect = true;
    BlockDirectory *dirblock = NULL;
    bool found = false;
    while(idPointBlock<SIZE_BLOCKS_INODE && !found){
        if(inodo->i_block[idPointBlock]!=-1){
            if(isDirect){
                dirblock = readBlockDirectory(path,getInitBlock(sb,inodo->i_block[idPointBlock]));
                if(strcmp(dirblock->b_content[0].b_name,nameDir)==0){
                    //BLOQUE ENCONTRADO
                    *indexBloqueActual = inodo->i_block[idPointBlock];
                     int indexBloque =2;
                     while (indexBloque<4) {
                         if(dirblock->b_content[indexBloque].b_inodo == -1){
                            *indexFree = indexBloque;
                            found = true;
                            break;
                         }
                         indexBloque++;
                     }
                }
            }else{
                //LEER APUNTADOR INDIRECTO
                *type = POINTERS;
                *indexBloqueActual = inodo->i_block[idPointBlock];
                int bloque = getFreeIndexFromBlockPointer(idPointBlock,inodo,*indexBloqueActual,path,sb,indexFree);
                writeInodo(inodo,path,getInitInode(sb,*indexInodoActual));
                if(bloque == -1){
                    idPointBlock++;
                    continue;
                }
                found = true;
                *indexBloqueActual = bloque;
            }
        }else{
            if(isDirect){
                //CREAR UN NUEVO BLOQUE
                BlockDirectory *nuevo = getNewBlockDir(dirblock->b_content[0].b_name,dirblock->b_content[0].b_inodo,
                dirblock->b_content[1].b_name,dirblock->b_content[1].b_inodo);
                inodo->i_block[idPointBlock] = sb->s_first_blo;
                writeBlockDirectory(nuevo,path,getInitBlock(sb,sb->s_first_blo));
                writeInodo(inodo,path,getInitInode(sb,*indexInodoActual));
                //restar un bloque y un inodo del super bloque
                *indexBloqueActual = sb->s_first_blo;
                *indexFree = 2;
                found = true;
                sb->s_first_blo = getBitmapIndex(sb->s_bm_block_start,sb->s_blocks_count,path);
                sb->s_free_blocks_count--;
            }else{
               //CREAR NUEVO BLOQUE
                int bloque = createPointersInd(idPointBlock,indexBloqueActual,sb,path);
                if(bloque== -1) return ERROR_LEVEL_FULL;
                inodo->i_block[idPointBlock] = bloque;
                writeInodo(inodo,path,getInitInode(sb,*indexInodoActual));
                //*indexBloqueActual = bloque;
                *indexFree = 0;
                *type = POINTERS;
                found = true;
            }

        }
        idPointBlock++;
        if(idPointBlock>11){
            isDirect = false;
        }
    }

    delete inodo;
    delete dirblock;
    return SUCCESS;
}

int createPointersInd(int idPointBlock,int *idBloqueActual,SuperBlock *sb,char path[]){
    if(idPointBlock-11 == 1) {
        BlockPointer *newPoints = getNewBlockPointer();
       int idBloque = sb->s_first_blo;
        writeBlockPointer(newPoints,path,getInitBlock(sb,sb->s_first_blo));
        sb->s_first_blo = getBitmapIndex(sb->s_bm_block_start,sb->s_blocks_count,path);
        sb->s_free_blocks_count--;
        *idBloqueActual = idBloque;
        return idBloque;
    }else{
        int res = createPointersInd(idPointBlock-1,idBloqueActual,sb,path);
        BlockPointer *pointers = getNewBlockPointer();
        pointers->b_pointers[0] = res;
        res = sb->s_first_blo;
        writeBlockPointer(pointers,path,getInitBlock(sb,sb->s_first_blo));
        sb->s_first_blo = getBitmapIndex(sb->s_bm_block_start,sb->s_blocks_count,path);
        sb->s_free_blocks_count--;
        return res;
    }
    return -1;
}

int getFreeIndexFromBlockPointer(int nivel,Inodo *inodo,int indexBloqueActual,char path[],SuperBlock *sb,int *indexFree){
        if(inodo->i_block[nivel]!=-1){
                if(nivel == 12){
                    BlockPointer *pointers = readBlockPointer(path,getInitBlock(sb,indexBloqueActual));
                        if(pointers == NULL){
                            return ERROR_DIR_NOT_EXIST;
                        }
                    int indexInBlockP;
                    for(indexInBlockP = 0;indexInBlockP<SIZE_BLOCKS_INODE;indexInBlockP++){
                        if(pointers->b_pointers[indexInBlockP]==-1){
                            *indexFree = indexInBlockP;
                            return indexBloqueActual;
                        }
                    }
                }else{
                    BlockPointer *pointers = readBlockPointer(path,getInitBlock(sb,indexBloqueActual));
                        if(pointers == NULL){
                            return ERROR_DIR_NOT_EXIST;
                        }
                    int indexInBlockP;
                    for(indexInBlockP = 0;indexInBlockP<SIZE_BLOCKS_INODE;indexInBlockP++){
                        if(pointers->b_pointers[indexInBlockP]!=-1){
                            int r = getFreeIndexFromBlockPointer(nivel-1,inodo,pointers->b_pointers[indexInBlockP],path,sb,indexFree);
                            if(r!=-1)return r;
                        }else{
                            int bloquePadre = indexBloqueActual;
                            int bloque = createPointersInd(nivel-1,&indexBloqueActual,sb,path);
                            if(bloque== -1) return ERROR_LEVEL_FULL;
                            pointers->b_pointers[indexInBlockP] = bloque;
                            writeBlockPointer(pointers,path,getInitBlock(sb,bloquePadre));
                            //*indexBloqueActual = bloque;
                            *indexFree = 0;
                            return bloque;
                        }
                    }
                }
        }
    return -1;
}

int writeDirectory(SuperBlock *sb,char path[],char nameDir[],char namePad[],int indexPad){
    //INODO DE CARPETA NUEVA
    Inodo *nuevo = getNewInodo(IN_DIRECTORY,777,-1);
    int indexI = sb->s_firts_ino;
    //BLOQUE DE CARPETA NUEVA
    BlockDirectory *dir = getNewBlockDir(nameDir,indexI,namePad,indexPad);
    //ASIGNANDO BLOQUE A CARPETA
    nuevo->i_block[0] = sb->s_first_blo;
    writeInodo(nuevo,path,getInitInode(sb,indexI));
    writeBlockDirectory(dir,path,getInitBlock(sb,sb->s_first_blo));
    delete nuevo;
    delete dir;
    sb->s_firts_ino = getBitmapIndex(sb->s_bm_inode_start,sb->s_inodes_count,path);
    sb->s_first_blo = getBitmapIndex(sb->s_bm_block_start,sb->s_blocks_count,path);
    sb->s_free_blocks_count--;
    sb->s_free_inodes_count--;
    return indexI;
}

BlockDirectory* getNewBlockDir(char name[],int indexDir,char namepad[],int indexPad){
    BlockDirectory *block= (BlockDirectory*)malloc(sizeof(BlockDirectory));
    int i;
    for(i=0;i<4;i++){
        clearArray(block->b_content[i].b_name,14);
        block->b_content[i].b_inodo = -1;
    }
    strcpy(block->b_content[0].b_name,name);
    block->b_content[0].b_inodo = indexDir;

    strcpy(block->b_content[1].b_name,namepad);
    block->b_content[1].b_inodo = indexPad;

    return block;
}

void writeSuperBlock(SuperBlock *sb,char path[],int init){
    FILE * myFile;
     myFile = fopen (path,"rb+");
     if (myFile==NULL)
     {
         cout<<"Error al abrir el disco\n";
         return;
     }
     fseek(myFile, init, SEEK_SET);
     fwrite(sb, sizeof(SuperBlock), 1, myFile);
     //cerrando stream
     fclose (myFile);
}

void writeBlockDirectory(BlockDirectory *sb,char path[],int init){
    FILE * myFile;
     myFile = fopen (path,"rb+");
     if (myFile==NULL)
     {
         cout<<"Error al abrir el disco\n";
         return;
     }
     fseek(myFile, init, SEEK_SET);
     fwrite(sb, sizeof(BlockDirectory), 1, myFile);
     //cerrando stream
     fclose (myFile);
}

void writeBlockFile(BlockFile *sb,char path[],int init){
    FILE * myFile;
     myFile = fopen (path,"rb+");
     if (myFile==NULL)
     {
         cout<<"Error al abrir el disco\n";
         return;
     }
     fseek(myFile, init, SEEK_SET);
     fwrite(sb, sizeof(BlockFile), 1, myFile);
     //cerrando stream
     fclose (myFile);
}

void writeBlockPointer(BlockPointer *newPoints,char path[],int init){
    FILE * myFile;
     myFile = fopen (path,"rb+");
     if (myFile==NULL)
     {
         cout<<"Error al abrir el disco\n";
         return;
     }
     fseek(myFile, init, SEEK_SET);
     fwrite(newPoints, sizeof(BlockPointer), 1, myFile);
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
    delete disco;
    FILE *myFile = fopen(path,"rb+");
    if(myFile==NULL){
        cout<<"Error al abrir el disco \n";
        return NULL;
    }
    SuperBlock *sb = (SuperBlock*)malloc(sizeof(SuperBlock));

    fseek(myFile, init, SEEK_SET);
    fread(sb, sizeof(SuperBlock), 1, myFile);
    fclose(myFile);
    return sb;
}

SuperBlock* readSuperBlock(char path[], char name[],int *startSb){
    MBR *disco = openMBR(path);
    if(disco==NULL){
        std::cout<<"Error al leer el disco\n";
        return NULL;
    }
    Response res = getStartPartition(disco,name,startSb);
    if(res!=SUCCESS){
        return NULL;
    }
    delete disco;
    FILE *myFile = fopen(path,"rb+");
    if(myFile==NULL){
        cout<<"Error al abrir el disco \n";
        return NULL;
    }
    SuperBlock *sb = (SuperBlock*)malloc(sizeof(SuperBlock));

    fseek(myFile, *startSb, SEEK_SET);
    fread(sb, sizeof(SuperBlock), 1, myFile);
    fclose(myFile);
    return sb;
}

void writeBitmap(int cantInodos,int init,char path[]){
    int contador = 0;
    FILE * myFile;
     myFile = fopen (path,"rb+");
     if (myFile==NULL)
     {
         cout<<"Error al abrir el disco\n";
         return;
     }
     fseek(myFile, init, SEEK_SET);
   while(contador<cantInodos){
        fwrite("0", sizeof(char), 1, myFile);
        contador++;
    }
    //cerrando stream
    fclose (myFile);
}

Inodo* getNewInodo(TypeInode type,int permisos,int size){
Inodo *nuevo = (Inodo*)malloc(sizeof(Inodo));
nuevo->i_uid=1;
nuevo->i_gid=1;
nuevo->i_size=size;
getCurrentDate(nuevo->i_atime);
getCurrentDate(nuevo->i_ctime);
getCurrentDate(nuevo->i_mtime);
int i;
for(i=0;i<SIZE_BLOCKS_INODE;i++){
    nuevo->i_block[i]=-1;
}
    nuevo->i_type = type;
    nuevo->i_perm =permisos;

return nuevo;
}

Inodo* readInodo(char path[], int init){
    FILE *myFile = fopen(path,"rb+");
    if(myFile==NULL){
        cout<<"Error al abrir el disco \n";
        return NULL;
    }
    Inodo *sb = (Inodo*)malloc(sizeof(Inodo));

    fseek(myFile, init, SEEK_SET);
    fread(sb, sizeof(Inodo), 1, myFile);
    fclose(myFile);
    if(sb->i_ctime[0]=='\0'){
        return NULL;
    }
    return sb;
}

BlockDirectory* readBlockDirectory(char path[], int init){
    FILE *myFile = fopen(path,"rb+");
    if(myFile==NULL){
        cout<<"Error al abrir el disco \n";
        return NULL;
    }
    BlockDirectory *bd = (BlockDirectory*)malloc(sizeof(BlockDirectory));

    fseek(myFile, init, SEEK_SET);
    fread(bd, sizeof(BlockDirectory), 1, myFile);
    fclose(myFile);
    if(bd->b_content[0].b_name[0]=='\0'){
        return NULL;
    }
    int i;
    for(i=0;i<4;i++){
        if(bd->b_content[i].b_name[0]=='\0'){
            bd->b_content[i].b_inodo=-1;
        }
    }
    return bd;
}

BlockFile* readBlockFile(char path[], int init){
    FILE *myFile = fopen(path,"rb+");
    if(myFile==NULL){
        cout<<"Error al abrir el disco \n";
        return NULL;
    }
    BlockFile *bd = (BlockFile*)malloc(sizeof(BlockFile));
    fseek(myFile, init, SEEK_SET);
    fread(bd, sizeof(BlockFile), 1, myFile);
    fclose(myFile);
    return bd;
}

BlockPointer* readBlockPointer(char path[], int init){
    FILE *myFile = fopen(path,"rb+");
    if(myFile==NULL){
        cout<<"Error al abrir el disco \n";
        return NULL;
    }
    BlockPointer *bd = (BlockPointer*)malloc(sizeof(BlockPointer));

    fseek(myFile, init, SEEK_SET);
    fread(bd, sizeof(BlockPointer), 1, myFile);
    fclose(myFile);
    return bd;
}

void writeInodo(Inodo *inodo, char path[], int init){
    FILE *myFile = fopen(path,"rb+");
    if(myFile==NULL){
        cout<<"Error al abrir el disco \n";
        return;
    }
    fseek(myFile, init, SEEK_SET);
    fwrite(inodo,sizeof(Inodo),1,myFile);
    fclose(myFile);
}

Response createFile(char newPath[], bool createPath, char pathFile[], char path[], char namePartition[]){
    FILE *fileText;
    fileText = fopen(pathFile,"r+");
    if(fileText==NULL){
        cout<<"Error al abrir el archivo en ruta \""<<pathFile<<"\"\n";
        return ERROR_UNHANDLED;
    }
     fseek(fileText, 0L, SEEK_END);
     int file_size = ftell(fileText);

     char txt[file_size];
     fseek(fileText,0L,SEEK_SET);
     fread(txt,file_size,1,fileText);

     fclose(fileText);
    return createFileWithText(newPath,createPath,txt,file_size,path,namePartition);
}

Response createFileWithText(char newPath[], bool createPath, char text[],int size, char path[], char namePartition[]){
    //VALIDAR QUE HAYA ESPACIO PARA CREAR INODOS Y BLOQUES
    int startSb;
    SuperBlock *sb = readSuperBlock(path,namePartition,&startSb);
    if(sb==NULL){
        return ERROR_UNHANDLED;
    }

    int indexInodoPadre = 0;
    std::stringstream ss(newPath);
        std::string token;
        std::string dirPad="/";
        int indexBloqueActual = 0;
        while (std::getline(ss, token, '/')) {
            if(token!=""){
                //cout<<"padre: "<<dirPad<<endl;
                //cout<<"archivo/carpeta: "<<token<<endl;
                if (ss.tellg() == -1) {
                    Response res = createChildFile(size,text,path,&dirPad[0],&token[0],sb,indexBloqueActual,indexInodoPadre);
                    if(res!=SUCCESS)return res;
                }else{
                    int indexBloque = findDirectory(&token[0],path,&indexInodoPadre,sb);
                    if(indexBloque!=-1){
                        indexBloqueActual = indexBloque;
                    }else{
                        if(createPath){
                            Response rs = createChildDirectory(&dirPad[0],&token[0],path,sb,&indexInodoPadre,&indexBloqueActual);;
                            if(rs!=SUCCESS){
                                return rs;
                            }
                        }else{
                            return ERROR_DIR_NOT_EXIST;
                        }
                    }
                }
                dirPad = token;
            }
        }
     writeSuperBlock(sb,path,startSb);
    delete sb;
    return SUCCESS;
}

Response createFile(char newPath[], bool createPath, int size,char path[],char namePartition[]){
    //generar texto
    char *txt = (char*)malloc(sizeof(char)*size);
    int i;
    char caracter = '0';
    for(i=0;i<size;i++){
        txt[i] =caracter;
        caracter++;
        if(caracter>'9'){
            caracter = '0';
        }
    }
    return createFileWithText(newPath,createPath,txt,size,path,namePartition);
}

Response createChildFile(int size,char *text,char path[],char dirPad[],char name[],SuperBlock *sb,int indexBloqueActual,int indexInodoPadre){
    int indexFree = -1;
    TypeBlock type = DIRECTORY;
        Response res = getFreeIndexDirectory(dirPad,path,sb,&indexBloqueActual,&indexInodoPadre,&indexFree,&type);
        if(res!=SUCCESS){
            return res;
        }
    Inodo *inodo = getNewInodo(IN_FILE,664,size);
    //BLOQUE DE ARCHIVO
    BlockFile *block = getNewBlockFile();
    int indexofInodo = 0;
    int contadorCaracteres = 0;
    int indexCaracteres = 0;
    bool keepSaving = true;
    while(keepSaving){
        if(contadorCaracteres>=64 || indexCaracteres>=size){
            if(indexofInodo<12){
                inodo->i_block[indexofInodo] = sb->s_first_blo;
                saveBlockFile(block,sb,path);
                indexofInodo++;
            }else{
               Response r = addFileBlockPointers(inodo,&indexofInodo,block,sb,path);
               if(r==ERROR_LEVEL_FULL){
                   if(indexofInodo<14){
                        indexofInodo++;
                        continue;
                   }
               }else if(r!=SUCCESS){
                   return r;
               }
            }
            block = getNewBlockFile();
            contadorCaracteres = 0;
        }
        block->b_content[contadorCaracteres] = text[indexCaracteres];
        if(indexCaracteres>=size){
            break;
        }
        contadorCaracteres++;
        indexCaracteres++;
    }
    writeInodo(inodo,path,getInitInode(sb,sb->s_firts_ino));

    BlockDirectory *blockPad = readBlockDirectory(path,getInitBlock(sb,indexBloqueActual));
    blockPad->b_content[indexFree].b_inodo = sb->s_firts_ino;
    strcpy(blockPad->b_content[indexFree].b_name,name);
    writeBlockDirectory(blockPad,path,getInitBlock(sb,indexBloqueActual));

    sb->s_firts_ino = getBitmapIndex(sb->s_bm_inode_start,sb->s_inodes_count,path);
    sb->s_free_inodes_count--;
    return SUCCESS;
}

Response addFileBlockPointers(Inodo *inodo,int *indexofInodo,BlockFile *block,SuperBlock *sb,char path[]){
    if(inodo->i_block[*indexofInodo]!=-1){
        int free= 0;
        int res =getFreeIndexFromBlockPointer(*indexofInodo,inodo,inodo->i_block[*indexofInodo],path,sb,&free);
        if(res==-1){
            return ERROR_LEVEL_FULL;
        }
        BlockPointer *point = readBlockPointer(path,getInitBlock(sb,res));
        point->b_pointers[free] = sb->s_first_blo;
        writeBlockPointer(point,path,getInitBlock(sb,res));
        saveBlockFile(block,sb,path);
    }else{
        //CREAR NUEVO BLOQUE
        int nuevoIndex;
        int bloque = createPointersInd(*indexofInodo,&nuevoIndex,sb,path);
         if(bloque== -1) return ERROR_LEVEL_FULL;
         inodo->i_block[*indexofInodo] = bloque;
         BlockPointer *point = readBlockPointer(path,getInitBlock(sb,nuevoIndex));
         point->b_pointers[0] = sb->s_first_blo;
         writeBlockPointer(point,path,getInitBlock(sb,nuevoIndex));
         //CREAR BLOQUE CON CONTENIDO
         saveBlockFile(block,sb,path);
    }
    return SUCCESS;
}

void saveBlockFile(BlockFile *block,SuperBlock *sb,char path[]){
    writeBlockFile(block,path,getInitBlock(sb,sb->s_first_blo));
    sb->s_first_blo = getBitmapIndex(sb->s_bm_block_start,sb->s_blocks_count,path);
    sb->s_free_blocks_count--;
}

BlockFile* getNewBlockFile(){
    BlockFile *bl = (BlockFile*)malloc(sizeof(BlockFile));
    clearArray(bl->b_content,64);;
    return bl;
}

BlockPointer* getNewBlockPointer(){
    BlockPointer *bl = (BlockPointer*)malloc(sizeof(BlockPointer));
    int j;
    for(j = 0;j<16;j++){
        bl->b_pointers[j]=-1;
    }
    return bl;
}

int getInitBlock(SuperBlock* sb,int index){
    return sb->s_block_start+(sb->s_block_size*index);
}

int getInitInode(SuperBlock *sb, int index){
    return sb->s_inode_start+(sb->s_inode_size*index);
}

Response catFile(char filePath[], char path[], char partition[]){
    char *content;
     char *title;
    Response res = findContentFile(filePath,path,partition,&content,&title);
    if(res!=SUCCESS)return res;
    //cout<<"--->"<<title<<"<---\n";
    cout<<content<<endl;
    return SUCCESS;
}

Response findContentFile(char filePath[], char path[], char partition[],char **content,char **title){
    int indexInode = findFile(filePath,path,partition,title);
    if(indexInode==-1){
        return ERROR_UNHANDLED;
    }
    int startSb = -1;
    SuperBlock *sb = readSuperBlock(path,partition,&startSb);
    if(sb==NULL){
        return ERROR_UNHANDLED;
    }
    Response res = getContentFile(indexInode,path,sb,content);
    delete sb;
    return res;
}

int findFile(char filePath[], char path[], char partition[],char **title){
    //VALIDAR QUE HAYA ESPACIO PARA CREAR INODOS Y BLOQUES
    int startSb;
    SuperBlock *sb = readSuperBlock(path,partition,&startSb);
    if(sb==NULL){
        return ERROR_UNHANDLED;
    }

    int indexInodoPadre = 0;
    std::stringstream ss(filePath);
        std::string token;
        std::string dirPad="/";
        int indexBloqueActual = 0;
        while (std::getline(ss, token, '/')) {
            if(token!=""){
                //cout<<"padre: "<<dirPad<<endl;
                //cout<<"carpeta: "<<token<<endl;
               if (ss.tellg() == -1) {
                   //inodo de directorio
                   Inodo *inodo = readInodo(path,getInitInode(sb,indexInodoPadre));
                    if(inodo == NULL){
                        return ERROR_DIR_NOT_EXIST;
                    }
                    if(inodo->i_type == IN_DIRECTORY){
                        int indexBloqueInodo;
                        for(indexBloqueInodo = 0;indexBloqueInodo<12;indexBloqueInodo++){
                            if(inodo->i_block[indexBloqueInodo]!=-1){
                                BlockDirectory *directory = readBlockDirectory(path,getInitBlock(sb,inodo->i_block[indexBloqueInodo]));
                                if(directory == NULL){
                                    return ERROR_UNHANDLED;
                                }
                                int indexInodoBloque;
                                for(indexInodoBloque = 2;indexInodoBloque <4;indexInodoBloque++){
                                    if(strcmp(directory->b_content[indexInodoBloque].b_name,&token[0])==0){
                                        //encontrado
                                        Inodo *file=readInodo(path,getInitInode(sb,directory->b_content[indexInodoBloque].b_inodo));
                                        if(file!=NULL){
                                            if(file->i_type == IN_FILE){
                                                *title=(char*)malloc(sizeof(directory->b_content[indexInodoBloque].b_name));
                                                strcpy(*title,directory->b_content[indexInodoBloque].b_name);
                                                return directory->b_content[indexInodoBloque].b_inodo;
                                            }
                                        }
                                        }
                                }
                            }
                        }
                    }else{
                        return -1;
                    }
                }else{
                    int indexBloque = findDirectory(&token[0],path,&indexInodoPadre,sb);
                    if(indexBloque!=-1){
                        indexBloqueActual = indexBloque;
                    }else{
                        showMessageError(ERROR_DIR_NOT_EXIST);
                        return -1;
                }
                dirPad = token;
            }
          }
        }
  return -1;
}

Response getContentFile(int indexInodo, char path[],SuperBlock *sb,char **content){
    Inodo *inodo = readInodo(path,getInitInode(sb,indexInodo));
    if(inodo->i_type == IN_FILE){
        (*content) =(char*)malloc(inodo->i_size);
        clearArray(*content,inodo->i_size);
        int indexBloque;
        for(indexBloque = 0;indexBloque<12;indexBloque++){
            if(inodo->i_block[indexBloque]!=-1){
                BlockFile *file = readBlockFile(path,getInitBlock(sb,inodo->i_block[indexBloque]));
                if(file== NULL){
                    return ERROR_UNHANDLED;
                }
                strcat((*content),file->b_content);
            }
        }
        //APUNTADORES INDIRECTOS

    }
    return SUCCESS;
}

Response addUser(char *path,char *partition,char usr[],char pwd[],char grp[]){
    char *content;
     char *title;
     char *filePath="/users.txt";
     //******************
     int indexInode = findFile(filePath,path,partition,&title);
     if(indexInode==-1){
         return ERROR_UNHANDLED;
     }
     int startSb = -1;
     SuperBlock *sb = readSuperBlock(path,partition,&startSb);
     if(sb==NULL){
         return ERROR_UNHANDLED;
     }
     Response res = getContentFile(indexInode,path,sb,&content);
     if(res!=SUCCESS)return res;
     //***************************
    Group *grpp=getGroup(grp,content);
    if(grpp == NULL){
        return ERROR_GROUP_NOT_EXISTS;
    }
    int contadorUsuarios = countUsers(content);
        User *user=getUser(usr,content);
        if(user != NULL){
            return ERROR_USER_EXISTS;
        }
    strcat(content,&to_string(contadorUsuarios+1)[0]);
    strcat(content,",U,");
    strcat(content,usr);
    strcat(content,",");
    strcat(content,grp);
    strcat(content,",");
    strcat(content,pwd);
    strcat(content,"\n");
    Response r = ReplaceContentFile(indexInode,content,path,partition);
    delete sb;
    return r;
    return SUCCESS;
}

Response addGroup(char *path,char *partition,char grp[]){
    char *content;
     char *title;
     char *filePath="/users.txt";
     //******************
     int indexInode = findFile(filePath,path,partition,&title);
     if(indexInode==-1){
         return ERROR_UNHANDLED;
     }
     int startSb = -1;
     SuperBlock *sb = readSuperBlock(path,partition,&startSb);
     if(sb==NULL){
         return ERROR_UNHANDLED;
     }
     Response res = getContentFile(indexInode,path,sb,&content);
     if(res!=SUCCESS)return res;
     //***************************
    int contadorGrupos=countGroups(content);
    /*Group *grpp=getGroup(grp,content);
    if(grpp != NULL){
        return ERROR_GROUP_EXISTS;
    }*/
    strcat(content,&to_string(contadorGrupos+1)[0]);
    strcat(content,",G,");
    strcat(content,grp);
    strcat(content,"\n");
    Response r = ReplaceContentFile(indexInode,content,path,partition);
    delete sb;
    return r;

    return SUCCESS;
}

int countUsers(char contentUsers[]){
    int contadortoken;
    std::stringstream ss(contentUsers);
    std::string token;
    int contadorUsuarios = 0;
    while (std::getline(ss, token, '\n')) {
        if(token!=""){
            contadortoken = 0;
            std::stringstream line(token);
               std::string tokenLine;
               while (std::getline(line, tokenLine, ',')) {
                   if(contadortoken == 0){
                       if(tokenLine == "0"){
                           continue;
                       }
                   }else if(contadortoken==1){
                       if(tokenLine == "U"){
                           contadorUsuarios++;
                       }else{
                           break;
                       }
                   }else{
                       break;
                   }
                   contadortoken++;
               }
        }
    }
    return contadorUsuarios;
}

int countGroups(char contentGroups[]){
    int contadortoken;
    std::stringstream ss(contentGroups);
    std::string token;
    int contadorGrupos = 0;
    while (std::getline(ss, token, '\n')) {
        if(token!=""){
            contadortoken = 0;
            std::stringstream line(token);
               std::string tokenLine;
               while (std::getline(line, tokenLine, ',')) {
                   if(contadortoken == 0){
                       if(tokenLine == "0"){
                           continue;
                       }
                   }else if(contadortoken==1){
                       if(tokenLine == "G"){
                           contadorGrupos++;
                       }else{
                           break;
                       }
                   }else{
                       break;
                   }
                   contadortoken++;
               }
        }
    }
    return contadorGrupos;
}

User* getUser(char usr[],char *contentUsers){
    int contadortoken;
    string nameUser(usr);
    User *user = NULL;
    std::stringstream ss(contentUsers);
    std::string token;

    while (std::getline(ss, token, '\n')) {
        user = new User();
        contadortoken = 0;
        std::stringstream line(token);
           std::string tokenLine;
           while (std::getline(line, tokenLine, ',')) {
               if(contadortoken == 0){
                   if(tokenLine == "0"){
                       continue;
                   }else{
                       user->id = tokenLine;
                   }
               }else if(contadortoken==1){
                   if(tokenLine != "U"){
                       user = NULL;
                       break;
                   }
               }else if(contadortoken == 2){
                   user->group = tokenLine;
               }else if(contadortoken == 3){
                   if(tokenLine == nameUser ){
                       user->name = tokenLine;
                   }else{
                       user = NULL;
                       break;
                   }
               }else if(contadortoken == 4){
                   user->pwd = tokenLine;
               }else{
                   break;
               }
               contadortoken++;
           }
           if(user!=NULL)return user;
    }
    return user;
}

Group* getGroup(char name[],char *contentUsers){
    int contadortoken;
    string nameG(name);
    Group *grp = NULL;
    std::stringstream ss(contentUsers);
    std::string token;

    while (std::getline(ss, token, '\n')) {
        grp = new Group();
        contadortoken = 0;
        std::stringstream line(token);
           std::string tokenLine;
           while (std::getline(line, tokenLine, ',')) {
               if(contadortoken == 0){
                   if(tokenLine == "0"){
                       continue;
                   }else{
                       grp->id = tokenLine;
                   }
               }else if(contadortoken==1){
                   if(tokenLine != "G"){
                       grp = NULL;
                       break;
                   }
               }else if(contadortoken == 2){
                   if(tokenLine == nameG ){
                       grp->name = tokenLine;
                   }else{
                       grp = NULL;
                       break;
                   }
               }else{
                   break;
               }
               contadortoken++;
           }
           if(grp!=NULL)return grp;
    }
    return grp;
}

User* getUser(char usr[]){
    char *content;
     char *title;
     char *filePath="/users.txt";
     Response res = findContentFile(filePath,active_sesion->path,active_sesion->namePartition,&content,&title);
     if(res!=SUCCESS)return NULL;
    return getUser(usr,content);
}

Response ReplaceContentFile(int indexInode,char *content,char path[],char namePart[]){
    int startSb = 0;
    SuperBlock *sb = readSuperBlock(path,namePart,&startSb);
    if(sb==NULL){
        return ERROR_UNHANDLED;
    }
    Inodo *inodo = readInodo(path,getInitInode(sb,indexInode));
    if(inodo==NULL){
         return ERROR_UNHANDLED;
    }
    BlockFile *block = getNewBlockFile();
    int indexofInodo = 0;
    int contadorCaracteres = 0;
    int indexCaracteres = 0;
    bool keepSaving = true;
    while(keepSaving){
        if(contadorCaracteres>=64 || content[indexCaracteres]=='\0'){
            if(indexofInodo<12){
                if(inodo->i_block[indexofInodo]!=-1){
                    writeBlockFile(block,path,getInitBlock(sb,inodo->i_block[indexofInodo]));
                    indexofInodo++;
                }else{
                    inodo->i_block[indexofInodo] = sb->s_first_blo;
                    saveBlockFile(block,sb,path);
                    indexofInodo++;
                }
            }else{
               /*Response r = addFileBlockPointers(inodo,&indexofInodo,block,sb,path);
               if(r==ERROR_LEVEL_FULL){
                   if(indexofInodo<14){
                        indexofInodo++;
                        continue;
                   }
               }else if(r!=SUCCESS){
                   return r;
               }*/
            }
            block = getNewBlockFile();
            contadorCaracteres = 0;
        }
        block->b_content[contadorCaracteres] = content[indexCaracteres];
        if( content[indexCaracteres]=='\0'){
            break;
        }
        contadorCaracteres++;
        indexCaracteres++;
    }
    writeInodo(inodo,path,getInitInode(sb,indexInode));
    return SUCCESS;
}

Response editFile(char pathFile[],char newCont[],char path[],char namePart[]){
    char *title;
    char *content;
    int indexInode = findFile(pathFile,path,namePart,&title);
    if(indexInode==-1){
        return ERROR_UNHANDLED;
    }
    int startSb = -1;
    SuperBlock *sb = readSuperBlock(path,namePart,&startSb);
    if(sb==NULL){
        return ERROR_UNHANDLED;
    }
    Response res = getContentFile(indexInode,path,sb,&content);
    if(res!=SUCCESS){
        return res;
    }
    strcat(content,newCont);
    return ReplaceContentFile(indexInode,content,path,namePart);
}

Response deleteUser(char path[], char partition[],char name[]){
     char *content;
     char *title;
     char *filePath="/users.txt";
     int indexInode = findFile(filePath,path,partition,&title);
     if(indexInode==-1){
         return ERROR_UNHANDLED;
     }
     int startSb = -1;
     SuperBlock *sb = readSuperBlock(path,partition,&startSb);
     if(sb==NULL){
         return ERROR_UNHANDLED;
     }
     Response res = getContentFile(indexInode,path,sb,&content);
     if(res!=SUCCESS)return res;
    //***********************************************************
     std::stringstream ss(content);
     std::string token;
     std::string newContent="";
     int contadorToken = 0;
     bool found = false;
     bool save=false;
     while (std::getline(ss, token, '\n')) {
         if(!found){
             //******************************
             std::stringstream ss2(token);
             std::string tokenLine;
             std::string newline="";
             contadorToken = 0;
             while (std::getline(ss2, tokenLine, ',')) {
                 if(token!=""){
                     if(contadorToken == 0){
                         if(tokenLine != "0"){
                             newline +="0,";
                         }else{
                             newline+=tokenLine;
                             newline+=",";
                         }
                     }else if(contadorToken == 1){
                         if(tokenLine != "U"){
                             break;
                         }else{
                             newline+="U,";
                         }
                     }else if(contadorToken == 2){
                         newline+=tokenLine;
                         newline+=",";
                         if(name == tokenLine){
                             save = true;
                             found = true;
                         }else{
                             break;
                         }
                     }else if(contadorToken == 3){
                         newline+=tokenLine;
                         newline+=",";
                     }else{
                         newline+=tokenLine;
                         newline+="\n";
                     }
                 }
                 contadorToken++;
             }
             if(save){
                 newContent+=newline;
                 newContent+="\n";
             }else{
                 newContent+=token;
                 newContent+="\n";
             }
             //******************************
         }else{
             newContent+=token;
             newContent+="\n";
         }
     }
     char *my_argument = const_cast<char*> (newContent.c_str());
    Response r = ReplaceContentFile(indexInode,my_argument,path,partition);
    delete sb;
    return r;
}

Response deleteGroup(char path[], char partition[],char name[]){
     char *content;
     char *title;
     char *filePath="/users.txt";
     int indexInode = findFile(filePath,path,partition,&title);
     if(indexInode==-1){
         return ERROR_UNHANDLED;
     }
     int startSb = -1;
     SuperBlock *sb = readSuperBlock(path,partition,&startSb);
     if(sb==NULL){
         return ERROR_UNHANDLED;
     }
     Response res = getContentFile(indexInode,path,sb,&content);
     if(res!=SUCCESS)return res;
    //***********************************************************
     std::stringstream ss(content);
     std::string token;
     std::string newContent="";
     int contadorToken = 0;
     bool found = false;
     bool save=false;
     while (std::getline(ss, token, '\n')) {
         if(!found){
             //******************************
             std::stringstream ss2(token);
             std::string tokenLine;
             std::string newline="";
             contadorToken = 0;
             while (std::getline(ss2, tokenLine, ',')) {
                 if(token!=""){
                     if(contadorToken == 0){
                         if(tokenLine != "0"){
                             newline +="0,";
                         }else{
                             newline+=tokenLine;
                             newline+=",";
                         }
                     }else if(contadorToken == 1){
                         if(tokenLine != "G"){
                             break;
                         }else{
                             newline+="G,";
                         }
                     }else if(contadorToken == 2){
                         newline+=tokenLine;
                         newline+="\n";
                         if(name == tokenLine){
                             save = true;
                             found = true;
                         }else{
                             break;
                         }
                     }
                 }
                 contadorToken++;
             }
             if(save){
                 newContent+=newline;
                 newContent+="\n";
             }else{
                 newContent+=token;
                 newContent+="\n";
             }
             //******************************
         }else{
             newContent+=token;
             newContent+="\n";
         }
     }
     char *my_argument = const_cast<char*> (newContent.c_str());
    Response r = ReplaceContentFile(indexInode,my_argument,path,partition);
    delete sb;
    return r;
}
