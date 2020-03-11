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
    sb->s_block_size = sizeof(Block);
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
    Inodo *nuevo = getNewInodoDir();
    writeInodo(nuevo,path,sb->s_inode_start);
    delete disco;
    delete nuevo;
    return SUCCESS;
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

Response createDir(bool createMk,char id[],char path[]){
    MountedDisk *disk =getMountedDisk(id);
    if(disk==NULL){
        return ERROR_UNHANDLED;
    }

    MountedPart *partition =getMountedPartition(id);
    if(partition==NULL){
        return ERROR_UNHANDLED;
    }

    //VALIDAR QUE HAYA ESPACIO PARA CREAR INODOS Y BLOQUES

    SuperBlock *sb = readSuperBlock(disk->path,partition->name);
    if(sb==NULL){
        return ERROR_UNHANDLED;
    }

    Inodo *inodoPivote=NULL;
    if(sb->s_free_inodes_count <sb->s_inodes_count){
       inodoPivote = readInodo(disk->path,sb->s_inode_start);
    }

    std::stringstream ss(path);
        std::string token;
        while (std::getline(ss, token, '/')) {
            if(token!=""){
                cout<<token<<endl;
                if(createMk){
                    //crear inodos

                }else{
                    //BUSCAR CARPETA EN INODO ACTUAL

                   // inodoPivote = newFirstInodo(sb,disk,partition);

                }
            }
        }



return SUCCESS;
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
    Inodo *pivote = readInodo(disk->path,sb->s_inode_start);
    if(pivote != NULL){
        graphInodo(pivote,myFile);
    }
    fputs("}\n",myFile);
    //cerrando stream
    fclose (myFile);
    string pathString(path_report);
    string command = "dot -Tpng report_tree.dot -o \""+pathString+"\"";//+"/report_disk.png";
    system(command.c_str());
}

void graphInodo(Inodo* inodo,FILE *myFile){
fputs("nodo1[ shape=plaintext label=< \n", myFile);
fputs("<table border='0' cellborder='1' cellspacing='0'>\n", myFile);
fputs("<tr><td colspan=\"3\">Inodo",myFile);
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
while(i<12){
    fputs("<tr><td bgcolor=\"#ffd1a8\">AD1</td><td bgcolor=\"#ffd1a8\">",myFile);
    fputs(&to_string(inodo->i_block[i])[0],myFile);
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
}
