#include "handlerfilesystem.h"

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
        sizeN/=4+sizeof(Inodo)+3*sizeof(BlockFile);
    }else if(sistem == ext3){
        sizeN/=4+sizeof(Inodo)+3*sizeof(BlockFile)+sizeof(Journal);
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
    int sizeInodos = sizeof(Inodo)*cantInodos;
    //BLOQUES
    SuperBlock *sb = (SuperBlock*)malloc(sizeof(SuperBlock));
    sb->s_filesystem_type = sistem;
    sb->s_inodes_count = cantInodos;
    sb->s_blocks_count = cantBloques;
    sb->s_free_blocks_count = 0;
    sb->s_free_inodes_count = 0;
    getCurrentDate(sb->s_mtime);
    getCurrentDate(sb->s_umtime);
    sb->s_mnt_count = 0;
    sb->s_magic = 0xEF53;
    sb->s_inode_size = sizeof(Inodo);
    sb->s_block_size = sizeof(BlockFile);
    sb->s_firts_ino = 0;
    sb->s_first_blo = 0;
    sb->s_bm_inode_start = initPart+sizeof(SuperBlock);
    sb->s_bm_block_start = sb->s_bm_inode_start+cantInodos;
    sb->s_inode_start = sb->s_bm_block_start + cantBloques;
    sb->s_block_start = sb->s_inode_start + sizeInodos;
    sb->s_free_blocks_count = cantBloques;
    sb->s_free_inodes_count = cantInodos;
    writeSuperBlock(sb,path,initPart);
    //BITMAP DE INODOS
    writeBitmap(cantInodos,sb->s_bm_inode_start,path);
    //BITMAP DE BLOQUES
    writeBitmap(cantBloques,sb->s_bm_block_start,path);
    //CREAR CARPETA RAIZ
    writeDirectory(sb,path,"/","/",0);
    writeSuperBlock(sb,path,initPart);
    delete disco;
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
                cout<<"padre: "<<dirPad<<endl;
                cout<<"carpeta: "<<token<<endl;
                if (ss.tellg() == -1) {
                    return createChildDirectory(&dirPad[0],&token[0],disk->path,sb,startSb,&indexInodoPadre,&indexBloqueActual);
                }else{
                    int indexBloque = findDirectory(&token[0],disk->path,&indexInodoPadre,sb);
                    if(indexBloque!=-1){
                        indexBloqueActual = indexBloque;
                    }else{
                        if(createMk){
                            Response res = createChildDirectory(&dirPad[0],&token[0],disk->path,sb,startSb,&indexInodoPadre,&indexBloqueActual);;
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
return SUCCESS;
}

int findDirectory(char namedir[],char path[],int *indexInodoActual,SuperBlock *sb){
    Inodo *inodo = readInodo(path,sb->s_inode_start+(sb->s_inode_size*(*indexInodoActual)));
    if(inodo==NULL){
        return -1;
    }
    int indexBlock;
    BlockDirectory *block;
    for(indexBlock = 0;indexBlock<12;indexBlock++){
        if(inodo->i_block[indexBlock]!=-1){
            block = readBlockDirectory(path,sb->s_block_start+(sb->s_block_size*inodo->i_block[indexBlock]));
            if(block == NULL){
                return -1;
            }
            int i;
            for(i=2;i<4;i++){
                if(block->b_content[i].b_inodo!=-1){
                    if(strcmp(block->b_content[i].b_name,namedir)==0){
                        *indexInodoActual = block->b_content[i].b_inodo;
                        inodo = readInodo(path,sb->s_inode_start+(sb->s_inode_size*(*indexInodoActual)));
                        if(inodo==NULL){
                            return -1;
                        }
                        return inodo->i_block[0];
                    }
                }
            }
        }
    }
    //buscar en simble
    //buscar en doble
    //buscar en triple
    return -1;
}

Response createChildDirectory(char dirPad[],char dirName[],char path[],SuperBlock *sb,int startSb,int *indexInodoPadre,int *indexBloqueActual){
    int indexFree = -1;
    Response res = getFreeIndexDirectory(dirPad,path,sb,indexBloqueActual,indexInodoPadre,&indexFree);
    if(res!=SUCCESS){
        return res;
    }
    int indexnew = writeDirectory(sb,path,dirName,dirPad,*indexInodoPadre);
    writeSuperBlock(sb,path,startSb);
    BlockDirectory *block = readBlockDirectory(path,sb->s_block_start+(sb->s_block_size*(*indexBloqueActual)));
    block->b_content[indexFree].b_inodo = indexnew;
    strcpy(block->b_content[indexFree].b_name,dirName);
    writeBlockDirectory(block,path,sb->s_block_start+(sb->s_block_size*(*indexBloqueActual)));
    *indexInodoPadre = indexnew;
    return SUCCESS;
}

Response getFreeIndexDirectory(char nameDir[],char path[],SuperBlock *sb,int *indexBloqueActual,int *indexInodoActual,int *indexFree){
    Inodo *inodo = NULL;
    inodo = readInodo(path,sb->s_inode_start+(sb->s_inode_size*(*indexInodoActual)));
    if(inodo==NULL){
        return ERROR_DIR_NOT_EXIST;
    }

    int idPointBlock=0;
    bool isDirect = true;
    BlockDirectory *dirblock = NULL;
    bool found = false;
    while(idPointBlock<16 && !found){
        if(inodo->i_block[idPointBlock]!=-1){
            if(isDirect){
                dirblock = readBlockDirectory(path,sb->s_block_start+(sb->s_block_size*inodo->i_block[idPointBlock]));
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
            }
        }else{
            if(isDirect){
                //CREAR UN NUEVO BLOQUE
                BlockDirectory *nuevo = getNewBlockDir(dirblock->b_content[0].b_name,dirblock->b_content[0].b_inodo,
                        dirblock->b_content[1].b_name,dirblock->b_content[1].b_inodo);
                int indexB = getBitmapIndex(sb->s_bm_block_start,sb->s_inode_start,path);
                if(indexB==-1){
                    return ERROR_UNHANDLED;
                }
                inodo->i_block[idPointBlock] = indexB;
                writeBlockDirectory(nuevo,path,sb->s_block_start+(indexB*sb->s_block_size));
                writeInodo(inodo,path,sb->s_inode_start+(sb->s_inode_size*(*indexInodoActual)));
                //restar un bloque y un inodo del super bloque
                sb->s_free_blocks_count = sb->s_free_blocks_count-1;
                *indexBloqueActual = indexB;
                *indexFree = 2;
                found = true;
            }

        }
        if(idPointBlock>11){
            isDirect = false;
        }
        idPointBlock++;
    }
    delete inodo;
    delete dirblock;
    return SUCCESS;
}

int writeDirectory(SuperBlock *sb,char path[],char nameDir[],char namePad[],int indexPad){
    //INODO DE CARPETA NUEVA
    int indexI = getBitmapIndex(sb->s_bm_inode_start,sb->s_bm_block_start,path);
    if(indexI == -1){
       return ERROR_UNHANDLED;
    }
    Inodo *nuevo = getNewInodoDir();
    //BLOQUE DE CARPETA NUEVA
    BlockDirectory *dir = getNewBlockDir(nameDir,indexI,namePad,indexPad);
    int indexB = getBitmapIndex(sb->s_bm_block_start,sb->s_inode_start,path);
    if(indexB==-1){
        return ERROR_UNHANDLED;
    }
    //ASIGNANDO BLOQUE A CARPETA
    nuevo->i_block[0] = indexB;


    writeInodo(nuevo,path,sb->s_inode_start+(indexI*sb->s_inode_size));
    writeBlockDirectory(dir,path,sb->s_block_start+(indexB*sb->s_block_size));
    //restar un bloque y un inodo del super bloque
    sb->s_free_blocks_count = sb->s_free_blocks_count-1;
    sb->s_free_inodes_count = sb->s_free_inodes_count-1;
    delete nuevo;
    delete dir;
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
     //escribir MBR en disco
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
     //escribir MBR en disco
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
     //escribir MBR en disco
     fseek(myFile, init, SEEK_SET);
     fwrite(sb, sizeof(BlockFile), 1, myFile);
     //cerrando stream
     fclose (myFile);
}

void writeBlockPointer(BlockPointer *sb,char path[],int init){
    FILE * myFile;
     myFile = fopen (path,"rb+");
     if (myFile==NULL)
     {
         cout<<"Error al abrir el disco\n";
         return;
     }
     //escribir MBR en disco
     fseek(myFile, init, SEEK_SET);
     fwrite(sb, sizeof(BlockPointer), 1, myFile);
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
     //escribir MBR en disco
     fseek(myFile, init, SEEK_SET);
   while(contador<cantInodos){
        fwrite("0", sizeof(char), 1, myFile);
        contador++;
    }
    //cerrando stream
    fclose (myFile);
}

Response reportBitmap(int rep, char path[], char name[],char path_rep[]){
    SuperBlock *sb = readSuperBlock(path,name);
    if(sb == NULL){
        return ERROR_UNHANDLED;
    }
    FILE * myFile;
     myFile = fopen (path,"rb+");
     if (myFile==NULL)
     {
         cout<<"Error al abrir el disco\n";
         return ERROR_UNHANDLED;
     }

     FILE * fileReport;
      fileReport = fopen (path_rep,"w+");
      if (fileReport==NULL)
      {
          cout<<"Error al crear archivo de reporte\n";
          return ERROR_UNHANDLED;
      }
    fseek(fileReport, 0, SEEK_SET);
    if(rep == 0){
        //reporte de inodos
        char caracter;
        int contador = 0;
         fseek(myFile, sb->s_bm_inode_start, SEEK_SET);
        while(contador<sb->s_inodes_count){
            fread(&caracter, sizeof(char), 1, myFile);
            fwrite(&caracter, sizeof(char), 1, fileReport);
            if((contador+1)%20==0 && contador!=0){
                fwrite("\n", sizeof(char), 1, fileReport);
            }
            contador++;
        }
    }else{
        //reporte de bloques
        char caracter;
        int contador = 0;
         fseek(myFile, sb->s_bm_block_start, SEEK_SET);
        while(contador<sb->s_blocks_count){
            fread(&caracter, sizeof(char), 1, myFile);
            fwrite(&caracter, sizeof(char), 1, fileReport);
            if((contador+1)%20==0 && contador!=0){
                fwrite("\n", sizeof(char), 1, fileReport);
            }
            contador++;
        }
    }
    fclose (fileReport);
    fclose (myFile);
    delete sb;
    return SUCCESS;
}

void reportSuperBlock(char path[], char name[], char path_report[]){
    SuperBlock *sb = readSuperBlock(path,name);
    if(sb==NULL){
        return;
    }
        FILE * myFile;
         myFile = fopen ("report_sb.dot","w+");
         if (myFile==NULL)
         {
             cout<<"Error al crear el archivo\n";
             return;
         }
         fseek(myFile, 0, SEEK_SET);
         fputs("digraph {\ntbl [\nshape=plaintext\n label=<\n", myFile);
         fputs("<table border='0' cellborder='1' cellspacing='0'>\n",myFile);
         fputs("<tr><td colspan=\"3\">",myFile);
         fputs(name,myFile);
         fputs("</td></tr>\n",myFile);
         fputs("<th><td>Nombre</td><td>Valor</td></th>\n",myFile);
         fputs("<tr><td bgcolor=\"#bcf7c1\">s_inodes_count</td><td bgcolor=\"#bcf7c1\">",myFile);
         fputs(&to_string(sb->s_inodes_count)[0],myFile);
         fputs("</td></tr>\n",myFile);
         fputs("<tr><td bgcolor=\"lightblue\">s_blocks_count</td><td bgcolor=\"lightblue\">",myFile);
         fputs(&to_string(sb->s_blocks_count)[0],myFile);
         fputs("</td></tr>\n",myFile);
         fputs("<tr><td bgcolor=\"#bcf7c1\">s_free_blocks_count</td><td bgcolor=\"#bcf7c1\">",myFile);
         fprintf(myFile, "%d", sb->s_free_blocks_count);
         fputs("</td></tr>\n",myFile);
         fputs("<tr><td bgcolor=\"lightblue\">s_free_inodes_count</td><td bgcolor=\"lightblue\">",myFile);
         fputs(&to_string(sb->s_free_inodes_count)[0],myFile);
         fputs("</td></tr>\n",myFile);
         fputs("<tr><td bgcolor=\"#bcf7c1\">s_mtime</td><td bgcolor=\"#bcf7c1\">",myFile);
         fputs(sb->s_mtime,myFile);
         fputs("</td></tr>\n",myFile);
         fputs("<tr><td bgcolor=\"lightblue\">s_umtime</td><td bgcolor=\"lightblue\">",myFile);
         fputs(sb->s_umtime,myFile);
         fputs("</td></tr>\n",myFile);
         fputs("<tr><td bgcolor=\"#bcf7c1\">s_mnt_count</td><td bgcolor=\"#bcf7c1\">",myFile);
         fputs(&to_string(sb->s_mnt_count)[0],myFile);
         fputs("</td></tr>\n",myFile);
         fputs("<tr><td bgcolor=\"lightblue\">s_magic</td><td bgcolor=\"lightblue\">",myFile);
         fputs(&to_string(sb->s_magic)[0],myFile);
         fputs("</td></tr>\n",myFile);
         fputs("<tr><td bgcolor=\"#bcf7c1\">s_inode_size</td><td bgcolor=\"#bcf7c1\">",myFile);
         fputs(&to_string(sb->s_inode_size)[0],myFile);
         fputs("</td></tr>\n",myFile);
         fputs("<tr><td bgcolor=\"lightblue\">s_block_size</td><td bgcolor=\"lightblue\">",myFile);
         fputs(&to_string(sb->s_block_size)[0],myFile);
         fputs("</td></tr>\n",myFile);
         fputs("<tr><td bgcolor=\"#bcf7c1\">s_bm_inode_start</td><td bgcolor=\"#bcf7c1\">",myFile);
         fputs(&to_string(sb->s_bm_inode_start)[0],myFile);
         fputs("</td></tr>\n",myFile);

         fputs("<tr><td bgcolor=\"lightblue\">s_bm_block_start</td><td bgcolor=\"lightblue\">",myFile);
         fputs(&to_string(sb->s_bm_block_start)[0],myFile);
         fputs("</td></tr>\n",myFile);
         fputs("<tr><td bgcolor=\"#bcf7c1\">s_inode_start</td><td bgcolor=\"#bcf7c1\">",myFile);
         fputs(&to_string(sb->s_inode_start)[0],myFile);
         fputs("</td></tr>\n",myFile);
         fputs("<tr><td bgcolor=\"lightblue\">s_block_start</td><td bgcolor=\"lightblue\">",myFile);
         fputs(&to_string(sb->s_block_start)[0],myFile);
         fputs("</td></tr>\n",myFile);

         fputs("</table>\n",myFile);
         fputs(">];\n\n", myFile);
         fputs("}\n",myFile);
         //cerrando stream
         fclose (myFile);
         string pathString(path_report);
         string command = "dot -Tpng report_sb.dot -o \""+pathString+"\"";//+"/report_mbr.png";
         system(command.c_str());
}

Inodo* getNewInodoDir(){
Inodo *nuevo = (Inodo*)malloc(sizeof(Inodo));
nuevo->i_uid=1;
nuevo->i_gid=1;
nuevo->i_size=-1;
getCurrentDate(nuevo->i_atime);
getCurrentDate(nuevo->i_ctime);
getCurrentDate(nuevo->i_mtime);
int i;
for(i=0;i<16;i++){
    nuevo->i_block[i]=-1;
}
    nuevo->i_type = Dir;
    nuevo->i_perm =777;

return nuevo;
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

    while (contador<finBm) {
        fread(&caracter,sizeof(char),1,myFile);
        if(caracter == '0'){
            fseek(myFile, -1, SEEK_CUR);
            fwrite("1",sizeof(char),1,myFile);
            break;
        }
        contador++;
    }
    fclose(myFile);
    return contador;
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

void reportTree(char path_report[], char id[]){
    MountedDisk *disk = getMountedDisk(id);
    if(disk == NULL){
        return;
    }
    MountedPart *partition = getMountedPartition(id);
    if(partition==NULL){
        return;
    }
    SuperBlock *sb = readSuperBlock(disk->path,partition->name);
    if(sb==NULL){
        return;
    }

    FILE * myFile;
     myFile = fopen ("report_tree.dot","w+");
     if (myFile==NULL)
     {
         cout<<"Error al crear el archivo de reporte\n";
         return;
     }
    fseek(myFile, 0, SEEK_SET);
    fputs("digraph G {\n", myFile);
    fputs("rankdir =LR;\n", myFile);
    //graphAllInodes(sb,myFile,disk->path);
    Inodo *inodo = readInodo(disk->path,sb->s_inode_start+(0*sb->s_inode_size));
    if(inodo!=NULL){
        graphInodo(inodo,0,myFile,disk->path,sb);
    }
    fputs("}\n",myFile);
    //cerrando stream
    fclose (myFile);
    string pathString(path_report);
    string command = "dot -Tpng report_tree.dot -o \""+pathString+"\"";//+"/report_disk.png";
    system(command.c_str());
}

void graphInodo(Inodo* inodo,int indexInodo,FILE *myFile,char path[],SuperBlock *sb){
    fputs("i_",myFile);
    fputs(&to_string(indexInodo)[0],myFile);
    fputs("[ shape=plaintext label=< \n", myFile);
    fputs("<table border='0' cellborder='1' cellspacing='0'>\n", myFile);
    fputs("<tr><td ",myFile);
        fputs("port=\"ib",myFile);
        fputs(&to_string(indexInodo*sizeof(Inodo))[0],myFile);
        fputs("\"",myFile);
    fputs(" colspan=\"3\">Inodo ",myFile);
    fputs(&to_string(indexInodo)[0],myFile);
    fputs("</td></tr>\n", myFile);
    fputs("<tr><td bgcolor=\"#fbffa8\">i_uid</td><td bgcolor=\"#fbffa8\">",myFile);
    fputs(&to_string(inodo->i_uid)[0],myFile);
    fputs("</td></tr>\n", myFile);
    fputs("<tr><td bgcolor=\"#fbffa8\">i_gid</td><td bgcolor=\"#fbffa8\">",myFile);
    fputs(&to_string(inodo->i_gid)[0],myFile);
    fputs("</td></tr>\n", myFile);
    fputs("<tr><td bgcolor=\"#fbffa8\">i_size</td><td bgcolor=\"#fbffa8\">",myFile);
    fputs(&to_string(inodo->i_size)[0],myFile);
    fputs("</td></tr>\n", myFile);
    fputs("<tr><td bgcolor=\"#fbffa8\">i_atime</td><td bgcolor=\"#fbffa8\">",myFile);
    fputs(inodo->i_atime,myFile);
    fputs("</td></tr>\n", myFile);
    fputs("<tr><td bgcolor=\"#fbffa8\">i_ctime</td><td bgcolor=\"#fbffa8\">",myFile);
    fputs(inodo->i_ctime,myFile);
    fputs("</td></tr>\n", myFile);
    fputs("<tr><td bgcolor=\"#fbffa8\">i_mtime</td><td bgcolor=\"#fbffa8\">",myFile);
    fputs(inodo->i_mtime,myFile);
    fputs("</td></tr>\n", myFile);
    fputs("<tr><td bgcolor=\"#fbffa8\">i_type</td><td bgcolor=\"#fbffa8\">",myFile);
    fputs(&to_string(inodo->i_type)[0],myFile);
    fputs("</td></tr>\n", myFile);
    fputs("<tr><td bgcolor=\"#fbffa8\">i_perm</td><td bgcolor=\"#fbffa8\">",myFile);
    fputs(&to_string(inodo->i_perm)[0],myFile);
    fputs("</td></tr>\n", myFile);
    int i=0;
      //APUNTADORES DIRECTOS
    while(i<12){
        fputs("<tr><td bgcolor=\"#ffd1a8\">AD", myFile);
        fputs(&to_string(i+1)[0],myFile);
        fputs("</td><td ",myFile);

        if(inodo->i_block[i]!=-1){
            fputs("port=\"",myFile);
            fputs(&to_string((indexInodo+inodo->i_block[i])*sizeof(Inodo))[0],myFile);
            fputs("\"",myFile);
        }

        fputs(" bgcolor=\"#ffd1a8\">",myFile);
        fputs(&to_string(inodo->i_block[i])[0],myFile);
        fputs("</td></tr>\n", myFile);
        i++;
    }

    while (i<16) {
      fputs("<tr><td bgcolor=\"#a8ffdf\">AI", myFile);
      fputs(&to_string(i-11)[0],myFile);
      fputs("</td><td bgcolor=\"#a8ffdf\">",myFile);
      fputs(&to_string(inodo->i_block[i])[0],myFile);
      fputs("</td></tr>\n", myFile);
    i++;
    }

    fputs("</table>\n",myFile);
    fputs(">];\n",myFile);

    //GRAFICAR BLOQUES
    i=0;
    while(i<12){
        if(inodo->i_block[i]!=-1){
            int pos = sb->s_block_start + (inodo->i_block[i]*sb->s_block_size);
            BlockDirectory *dir = readBlockDirectory(path,pos);
            if(dir!=NULL){
                graphBlockDirectory(dir,inodo->i_block[i],myFile,indexInodo,sb,path);
                graphConnectionInodoBloque(indexInodo,inodo->i_block[i],(indexInodo+inodo->i_block[i])*sizeof(Inodo),myFile);
            }
        }
       i++;
    }
}

void graphConnectionInodoBloque(int indexnodo,int indexBloque,int indexPuerto,FILE *myFile){
    fputs("i_",myFile);
    fputs(&to_string(indexnodo)[0],myFile);
    fputs(":",myFile);
    fputs(&to_string(indexPuerto)[0],myFile);
    fputs("->",myFile);
    fputs("b_",myFile);
    fputs(&to_string(indexBloque)[0],myFile);
    fputs(":",myFile);
    fputs(&to_string(indexPuerto)[0],myFile);

    fputs(";\n",myFile);
}

void graphConnectionBloqueInodo(int indexnodo,int indexBloque,int indexPuerto,FILE *myFile){
    fputs("b_",myFile);
    fputs(&to_string(indexBloque)[0],myFile);
    fputs(":",myFile);
    fputs("ib",myFile);
    fputs(&to_string(indexPuerto)[0],myFile);
    fputs("->",myFile);
    fputs("i_",myFile);
    fputs(&to_string(indexnodo)[0],myFile);
    fputs(":",myFile);
    fputs("ib",myFile);
    fputs(&to_string(indexPuerto)[0],myFile);
    fputs(";\n",myFile);
}

void graphBlockDirectory(BlockDirectory *block,int initBlock, FILE *myFile,int indexInodo,SuperBlock *sb,char path[]){
    fputs("b_",myFile);
    fputs(&to_string(initBlock)[0],myFile);
    fputs("[ shape=plaintext label=< \n", myFile);
    fputs("<table border='0' cellborder='1' cellspacing='0'>\n", myFile);
    fputs("<tr><td ",myFile);
        fputs("port=\"",myFile);
        fputs(&to_string((indexInodo+initBlock)*sizeof(Inodo))[0],myFile);
        fputs("\"",myFile);
    fputs(" colspan=\"3\">Bloque ",myFile);
    fputs(&to_string(initBlock)[0],myFile);
    fputs("</td></tr>\n", myFile);

    char colors[4][10];
    clearArray(colors[0],10);
    strcat(colors[0],"#e5b7ff");
    clearArray(colors[1],10);
    strcat(colors[1],"#c0ffb7");
    clearArray(colors[2],10);
    strcat(colors[2],"#ffb7c0");
    clearArray(colors[3],10);
    strcat(colors[3],"#ffd2b7");

    int i;
    for(i=0;i<4;i++){
        fputs("<tr><td bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\">b_name</td><td bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\" ",myFile);
        if(block->b_content[i].b_inodo!=-1 && block->b_content[i].b_inodo!=indexInodo){
            fputs("port=\"ib",myFile);
            fputs(&to_string(block->b_content[i].b_inodo*sizeof(Inodo))[0],myFile);
            fputs("\"",myFile);
        }
        fputs(">",myFile);
        fputs(block->b_content[i].b_name,myFile);
        fputs("</td></tr>\n",myFile);
        fputs("<tr><td bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\">b_inodo</td><td bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\">",myFile);
        fputs(&to_string(block->b_content[i].b_inodo)[0],myFile);
        fputs("</td></tr>\n",myFile);
    }

    fputs("</table>\n",myFile);
    fputs(">];\n",myFile);
    for(i=2;i<4;i++){
        if(block->b_content[i].b_inodo!=-1){
            Inodo *inodo = readInodo(path,sb->s_inode_start+(sb->s_inode_size*block->b_content[i].b_inodo));
            graphInodo(inodo,block->b_content[i].b_inodo,myFile,path,sb);
            graphConnectionBloqueInodo(block->b_content[i].b_inodo,initBlock,block->b_content[i].b_inodo*sizeof(Inodo),myFile);

        }
    }

}
