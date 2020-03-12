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
    sb->s_free_blocks_count = cantBloques-1;
    sb->s_free_inodes_count = cantInodos-1;
    writeSuperBlock(sb,path,initPart);
    //BITMAP DE INODOS
    writeBitmap(cantInodos,sb->s_bm_inode_start,path);
    //BITMAP DE BLOQUES
    writeBitmap(cantBloques,sb->s_bm_block_start,path);
    //CREAR CARPETA RAIZ
    createDirectory(sb,path,"/");
    delete disco;

    return SUCCESS;
}

Response createDirectory(SuperBlock *sb,char path[],char nameDir[]){
    int indexI = getBitmapIndex(sb->s_bm_inode_start,sb->s_bm_block_start,path);
    if(indexI == -1){
       return ERROR_UNHANDLED;
    }
    Inodo *nuevo = getNewInodoDir();
    BlockDirectory *dir = getNewBlockDir(indexI,nameDir);
    int indexB = getBitmapIndex(sb->s_bm_block_start,sb->s_inode_start,path);
    if(indexB==-1){
        return ERROR_UNHANDLED;
    }

    nuevo->i_block[0] = indexB;

    writeInodo(nuevo,path,sb->s_inode_start+(indexI*sb->s_inode_size));
    writeBlockDirectory(dir,path,sb->s_block_start+(indexB*sb->s_block_size));
    //Response res = addnewInodo(nuevo,index,DIRECTORY,dir,NULL,path,sb,nameDir);
    delete nuevo;
    delete dir;
    return SUCCESS;
}


BlockDirectory* getNewBlockDir(int indexInodo,char name[]){
    BlockDirectory *block= (BlockDirectory*)malloc(sizeof(BlockDirectory));
    int i;
    for(i=0;i<4;i++){
        block->b_content[i].b_inodo = -1;
    }
    block->b_content[0].b_inodo = indexInodo;
    strcpy(block->b_content[0].b_name,name);
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
    BlockDirectory *sb = (BlockDirectory*)malloc(sizeof(BlockDirectory));

    fseek(myFile, init, SEEK_SET);
    fread(sb, sizeof(BlockDirectory), 1, myFile);
    fclose(myFile);
    if(sb->b_content[0].b_name[0]=='\0'){
        return NULL;
    }
    return sb;
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
        graphInodo(inodo,0,myFile,sb->s_block_size,sb->s_block_start,disk->path);
    }
    fputs("}\n",myFile);
    //cerrando stream
    fclose (myFile);
    string pathString(path_report);
    string command = "dot -Tpng report_tree.dot -o \""+pathString+"\"";//+"/report_disk.png";
    system(command.c_str());
}

void graphAllInodes(SuperBlock *sb,FILE *file_report,char path[]){
    FILE * file_bitmap;
     file_bitmap = fopen (path,"rb+");
     if (file_bitmap==NULL)
     {
         cout<<"Error al abrir el disco\n";
         return;
     }
     //reporte de inodos
     char caracter;
     int contador = 0;
     fseek(file_bitmap, sb->s_bm_inode_start, SEEK_SET);
     while(contador<sb->s_inodes_count){
         fread(&caracter, sizeof(char), 1, file_bitmap);
         if(caracter == '1'){
             Inodo *inodo = readInodo(path,sb->s_inode_start+(contador*sb->s_inode_size));
             if(inodo!=NULL){
                 graphInodo(inodo,contador,file_report,sb->s_block_size,sb->s_block_start,path);
             }
             contador++;
         }
     }
    fclose(file_bitmap);
}

void graphInodo(Inodo* inodo,int indexInodo,FILE *myFile,int sizeBlock,int initBlocks,char path[]){
    fputs("i_",myFile);
    fputs(&to_string(indexInodo)[0],myFile);
    fputs("[ shape=plaintext label=< \n", myFile);
    fputs("<table border='0' cellborder='1' cellspacing='0'>\n", myFile);
    fputs("<tr><td colspan=\"3\">Inodo ",myFile);
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

    FILE * file_aux;
     file_aux = fopen ("aux_blocks.txt","w+");
     if (file_aux==NULL)
     {
         cout<<"Error al crear auxiliar\n";
         return;
     }

     FILE * file_conections;
      file_conections = fopen ("aux_conections.txt","w+");
      if (file_conections==NULL)
      {
          cout<<"Error al crear auxiliar\n";
          return;
      }

      //APUNTADORES DIRECTOS
    while(i<12){
        fputs("<tr><td bgcolor=\"#ffd1a8\">AD1</td><td port=\"",myFile);
        fputs(&to_string(indexInodo*sizeof(Inodo))[0],myFile);
        fputs("\" bgcolor=\"#ffd1a8\">",myFile);
        fputs(&to_string(inodo->i_block[i])[0],myFile);
        if(inodo->i_block[i]!=-1){
            BlockDirectory *dir = readBlockDirectory(path,initBlocks + (inodo->i_block[i]*sizeBlock));
            if(dir!=NULL){
                graphBlockDirectory(dir,i,file_aux,indexInodo);
                int j;
                for(j=0;j<4;j++){
                    if(dir->b_content[j].b_inodo!=-1){
                        writeConnection(dir->b_content[j].b_inodo,inodo->i_block[i],indexInodo*sizeof(Inodo),file_conections);
                    }
                }
            }
        }
        fputs("</td></tr>\n", myFile);
        i++;
    }

    while (i<16) {
      fputs("<tr><td bgcolor=\"#a8ffdf\">AI1</td><td bgcolor=\"#a8ffdf\">",myFile);
      fputs(&to_string(inodo->i_block[i])[0],myFile);
      fputs("</td></tr>\n", myFile);
    i++;
    }

    fputs("</table>\n",myFile);
    fputs(">];\n",myFile);
    //ESCRIBIR BLOQUES DE DIRECTORIOS
    fseek(file_aux,0,SEEK_SET);
    char linea[1024];
    while(fgets(linea, 1024, (FILE*) file_aux)) {
       fputs(linea,myFile);
    }
    fseek(file_conections,0,SEEK_SET);
    while(fgets(linea, 1024, (FILE*) file_conections)) {
       fputs(linea,myFile);
    }
    fclose(file_aux);
}

void writeConnection(int indexnodo,int indexBloque,int indexPuerto,FILE *myFile){
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

void graphBlockDirectory(BlockDirectory *block, int initBlock, FILE *myFile,int indexInodo){
    fputs("b_",myFile);
    fputs(&to_string(initBlock)[0],myFile);
    fputs("[ shape=plaintext label=< \n", myFile);
    fputs("<table border='0' cellborder='1' cellspacing='0'>\n", myFile);
    fputs("<tr><td colspan=\"3\">Bloque ",myFile);
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
        fputs("<tr><td  port=\"",myFile);
        fputs(&to_string(indexInodo*sizeof(Inodo))[0],myFile);
        fputs("\" bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\">b_name</td><td bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\">",myFile);
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
}
