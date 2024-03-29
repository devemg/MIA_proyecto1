#include "handler_reports.h"

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
         //file system
         fputs("<tr><td bgcolor=\"#bcf7c1\">s_filesystem_type</td><td bgcolor=\"#bcf7c1\">",myFile);
         fputs(&to_string(sb->s_filesystem_type)[0],myFile);
         fputs("</td></tr>\n",myFile);
         //count inodes
         fputs("<tr><td bgcolor=\"#b4e0fa\">s_inodes_count</td><td bgcolor=\"#b4e0fa\">",myFile);
         fputs(&to_string(sb->s_inodes_count)[0],myFile);
         fputs("</td></tr>\n",myFile);
         //count blocks
         fputs("<tr><td bgcolor=\"#bcf7c1\">s_blocks_count</td><td bgcolor=\"#bcf7c1\">",myFile);
         fputs(&to_string(sb->s_blocks_count)[0],myFile);
         fputs("</td></tr>\n",myFile);
         //count free inodes
         fputs("<tr><td bgcolor=\"#b4e0fa\">s_free_inodes_count</td><td bgcolor=\"#b4e0fa\">",myFile);
         fputs(&to_string(sb->s_free_inodes_count)[0],myFile);
         fputs("</td></tr>\n",myFile);
         //count free blocks
         fputs("<tr><td bgcolor=\"#bcf7c1\">s_free_blocks_count</td><td bgcolor=\"#bcf7c1\">",myFile);
         fprintf(myFile, "%d", sb->s_free_blocks_count);
         fputs("</td></tr>\n",myFile);
         //date mount
         fputs("<tr><td bgcolor=\"#b4e0fa\">s_mtime</td><td bgcolor=\"#b4e0fa\">",myFile);
         fputs(sb->s_mtime,myFile);
         fputs("</td></tr>\n",myFile);
         //date unmount
         fputs("<tr><td bgcolor=\"#bcf7c1\">s_umtime</td><td bgcolor=\"#bcf7c1\">",myFile);
         fputs(sb->s_umtime,myFile);
         fputs("</td></tr>\n",myFile);
         //count mount
         fputs("<tr><td bgcolor=\"#b4e0fa\">s_mnt_count</td><td bgcolor=\"#b4e0fa\">",myFile);
         fputs(&to_string(sb->s_mnt_count)[0],myFile);
         fputs("</td></tr>\n",myFile);
         //magic
         fputs("<tr><td bgcolor=\"#bcf7c1\">s_magic</td><td bgcolor=\"#bcf7c1\">",myFile);
         fputs(&to_string(sb->s_magic)[0],myFile);
         fputs("</td></tr>\n",myFile);
         //size inode
         fputs("<tr><td bgcolor=\"#b4e0fa\">s_inode_size</td><td bgcolor=\"#b4e0fa\">",myFile);
         fputs(&to_string(sb->s_inode_size)[0],myFile);
         fputs("</td></tr>\n",myFile);
         //size block
         fputs("<tr><td bgcolor=\"#bcf7c1\">s_block_size</td><td bgcolor=\"#bcf7c1\">",myFile);
         fputs(&to_string(sb->s_block_size)[0],myFile);
         fputs("</td></tr>\n",myFile);
         //first free inode
         fputs("<tr><td bgcolor=\"#b4e0fa\">s_firts_ino</td><td bgcolor=\"#b4e0fa\">",myFile);
         fputs(&to_string(sb->s_firts_ino)[0],myFile);
         fputs("</td></tr>\n",myFile);
         //first free block
         fputs("<tr><td bgcolor=\"#bcf7c1\">s_first_blo</td><td bgcolor=\"#bcf7c1\">",myFile);
         fputs(&to_string(sb->s_first_blo)[0],myFile);
         fputs("</td></tr>\n",myFile);
         //start bitmap inodes
         fputs("<tr><td bgcolor=\"#b4e0fa\">s_bm_inode_start</td><td bgcolor=\"#b4e0fa\">",myFile);
         fputs(&to_string(sb->s_bm_inode_start)[0],myFile);
         fputs("</td></tr>\n",myFile);
         //start bitmap blocks
         fputs("<tr><td bgcolor=\"#bcf7c1\">s_bm_block_start</td><td bgcolor=\"#bcf7c1\">",myFile);
         fputs(&to_string(sb->s_bm_block_start)[0],myFile);
         fputs("</td></tr>\n",myFile);
         //start inodes
         fputs("<tr><td bgcolor=\"#b4e0fa\">s_inode_start</td><td bgcolor=\"#b4e0fa\">",myFile);
         fputs(&to_string(sb->s_inode_start)[0],myFile);
         fputs("</td></tr>\n",myFile);
         //start blocks
         fputs("<tr><td bgcolor=\"#bcf7c1\">s_block_start</td><td bgcolor=\"#bcf7c1\">",myFile);
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
    Inodo *inodo = readInodo(disk->path,getInitInode(sb,0));
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
    if(inodo==NULL) return;
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

    while (i<SIZE_BLOCKS_INODE) {
      fputs("<tr><td bgcolor=\"#a8ffdf\">AI", myFile);
      fputs(&to_string(i-11)[0],myFile);
      fputs("</td><td ",myFile);
      if(inodo->i_block[i]!=-1){
          fputs("port=\"",myFile);
          fputs(&to_string((indexInodo+inodo->i_block[i])*sizeof(Inodo))[0],myFile);
          fputs("\"",myFile);
      }
      fputs(" bgcolor=\"#a8ffdf\">",myFile);
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
            int pos = getInitBlock(sb,inodo->i_block[i]);
                if(inodo->i_type == IN_DIRECTORY){
                    BlockDirectory *dir = readBlockDirectory(path,pos);
                    if(dir!=NULL){
                        graphBlockDirectory(dir,inodo->i_block[i],myFile,indexInodo,sb,path,true);
                    }
                }else if(inodo->i_type == IN_FILE){
                    BlockFile *dir = readBlockFile(path,pos);
                    if(dir!=NULL){
                        graphBlockFile(dir,inodo->i_block[i],myFile,indexInodo);
                    }
                }
             graphConnectionInodoBloque(indexInodo,inodo->i_block[i],(indexInodo+inodo->i_block[i])*sizeof(Inodo),myFile);
        }
       i++;
    }
    while(i<SIZE_BLOCKS_INODE){
        if(inodo->i_block[i]!=-1){
            graphBlockPointer(i-11,indexInodo,inodo->i_block[i],myFile,path,sb,true,inodo->i_type);
            graphConnectionInodoBloque(indexInodo,inodo->i_block[i],(indexInodo+inodo->i_block[i])*sizeof(Inodo),myFile);
        }
        i++;
    }
}

void graphBlockPointer(int level,int indexPadre,int indexBlock,FILE *fileReport,char path[],SuperBlock *sb,
                       bool graphConnection,TypeInode type){
    BlockPointer *block = readBlockPointer(path,getInitBlock(sb,indexBlock));
    if(block==NULL) return;
    fputs("b_",fileReport);
    fputs(&to_string(indexBlock)[0],fileReport);
    fputs("[ shape=plaintext label=< \n", fileReport);
    fputs("<table border='0' cellborder='1' cellspacing='0'>\n", fileReport);
    fputs("<tr><td ",fileReport);

    fputs("port=\"",fileReport);
    fputs(&to_string((indexPadre+indexBlock)*sizeof(Inodo))[0],fileReport);
    fputs("\"",fileReport);

    fputs(">Bloque ",fileReport);
    fputs(&to_string(indexBlock)[0],fileReport);
    fputs("</td></tr>\n", fileReport);
    int i;
    for(i=0;i<SIZE_BLOCKS_INODE;i++){
        fputs("<tr><td bgcolor = \"#FFA07A\" ",fileReport);
        if(block->b_pointers[i]!=-1){
            if(level == 1 && type == IN_DIRECTORY){
                fputs("port=\"ib",fileReport);
                fputs(&to_string(block->b_pointers[i]*sizeof(Inodo))[0],fileReport);
                fputs("\"",fileReport);

            }else{
                fputs("port=\"",fileReport);
                int port = (indexBlock+block->b_pointers[i])*sizeof(Inodo);
                fputs(&to_string(port)[0],fileReport);
                fputs("\"",fileReport);
            }
        }
        fputs(">",fileReport);
        fputs(&to_string(block->b_pointers[i])[0],fileReport);
        fputs("</td></tr>\n",fileReport);
    }
    fputs("</table>\n",fileReport);
    fputs(">];\n",fileReport);

        for(i=0;i<SIZE_BLOCKS_INODE;i++){
            if(block->b_pointers[i]!=-1){
                if(level==1){
                    if(type==IN_DIRECTORY){
                        Inodo *ind = readInodo(path,getInitInode(sb,block->b_pointers[i]));
                            graphInodo(ind,block->b_pointers[i],fileReport,path,sb);
                            if(graphConnection)
                            graphConnectionBloqueInodo(block->b_pointers[i],indexBlock,block->b_pointers[i]*sizeof(Inodo),fileReport);
                    }else{
                        BlockFile *file = readBlockFile(path,getInitBlock(sb,block->b_pointers[i]));
                        graphBlockFile(file,block->b_pointers[i],fileReport,indexBlock);
                        int port = (indexBlock+block->b_pointers[i])*sizeof(Inodo);
                        if(graphConnection)
                        graphConnectionBloqueBLoque(indexBlock,block->b_pointers[i],port,fileReport);
                    }
                }else{
                       graphBlockPointer(level-1,indexBlock,block->b_pointers[i],fileReport,path,sb,graphConnection,type);
                       int port = (indexBlock+block->b_pointers[i])*sizeof(Inodo);
                       if(graphConnection)
                       graphConnectionBloqueBLoque(indexBlock,block->b_pointers[i],port,fileReport);
                    }
                }
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

void graphConnectionBloqueBLoque(int indexB1,int indexB2,int indexPuerto,FILE *myFile){
    fputs("b_",myFile);
    fputs(&to_string(indexB1)[0],myFile);
    fputs(":",myFile);
    fputs(&to_string(indexPuerto)[0],myFile);
    fputs("->",myFile);
    fputs("b_",myFile);
    fputs(&to_string(indexB2)[0],myFile);
    fputs(":",myFile);
    fputs(&to_string(indexPuerto)[0],myFile);
    fputs(";\n",myFile);
}

void graphBlockDirectory(BlockDirectory *block,int initBlock, FILE *myFile,int indexInodo,SuperBlock *sb,char path[],bool graphConnection){
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
    if(graphConnection){
    for(i=2;i<4;i++){
        if(block->b_content[i].b_inodo!=-1){
            Inodo *inodo = readInodo(path,getInitInode(sb,block->b_content[i].b_inodo));
            graphInodo(inodo,block->b_content[i].b_inodo,myFile,path,sb);
            graphConnectionBloqueInodo(block->b_content[i].b_inodo,initBlock,block->b_content[i].b_inodo*sizeof(Inodo),myFile);
        }
    }
    }

}

void graphBlockFile(BlockFile *block,int initBlock, FILE *myFile,int indexInodo){
    fputs("b_",myFile);
    fputs(&to_string(initBlock)[0],myFile);
    fputs("[ shape=plaintext label=< \n", myFile);
    fputs("<table border='0' cellborder='1' cellspacing='0'>\n", myFile);
    fputs("<tr><td ",myFile);
        fputs("port=\"",myFile);
        fputs(&to_string((indexInodo+initBlock)*sizeof(Inodo))[0],myFile);
        fputs("\">Bloque ",myFile);
    fputs(&to_string(initBlock)[0],myFile);
    fputs("</td></tr>\n", myFile);
    fputs("<tr><td>",myFile);
    int i;
    for(i=0;i<64;i++){
        if(block->b_content[i]!='\0' && block->b_content[i]>0){
           fprintf(myFile,"%c", block->b_content[i]);
           if(i!=0){
               if(i%31 == 0){
                  fputs("<br/>",myFile);
               }
           }
        }
    }
    fputs("</td></tr>\n",myFile);
    fputs("</table>\n",myFile);
    fputs(">];\n",myFile);
}

Response reportInodes(char path[], char name[], char path_report[]){
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
      fileReport = fopen ("report_inodes.dot","w+");
      if (fileReport==NULL)
      {
          cout<<"Error al crear archivo de reporte\n";
          return ERROR_UNHANDLED;
      }
      fseek(fileReport, 0, SEEK_SET);
      fputs("digraph {\n", fileReport);
            //reporte de inodos
            char caracter;
            int contador = 0;
            Inodo *inodo;
             fseek(myFile, sb->s_bm_inode_start, SEEK_SET);
            while(contador<sb->s_inodes_count){
                fread(&caracter, sizeof(char), 1, myFile);
                //******
                if(caracter == '1'){
                    inodo = readInodo(path,getInitInode(sb,contador));
                    if(inodo == NULL){
                        break;
                    }
                    fputs("i_",fileReport);
                    fputs(&to_string(contador)[0],fileReport);
                    fputs("[ shape=plaintext label=< \n", fileReport);
                    fputs("<table border=\"1\" cellborder = \"0\" cellspacing='0'>\n", fileReport);
                    fputs("<tr><td colspan=\"3\">Inodo ",fileReport);
                    fputs(&to_string(contador)[0],fileReport);
                    fputs("</td></tr>\n", fileReport);
                    fputs("<tr><td bgcolor=\"#DAF7A6\">i_uid</td><td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(&to_string(inodo->i_uid)[0],fileReport);
                    fputs("</td></tr>\n", fileReport);
                    fputs("<tr><td bgcolor=\"#DAF7A6\">i_gid</td><td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(&to_string(inodo->i_gid)[0],fileReport);
                    fputs("</td></tr>\n", fileReport);
                    fputs("<tr><td bgcolor=\"#DAF7A6\">i_size</td><td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(&to_string(inodo->i_size)[0],fileReport);
                    fputs("</td></tr>\n", fileReport);
                    fputs("<tr><td bgcolor=\"#DAF7A6\">i_atime</td><td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(inodo->i_atime,fileReport);
                    fputs("</td></tr>\n", fileReport);
                    fputs("<tr><td bgcolor=\"#DAF7A6\">i_ctime</td><td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(inodo->i_ctime,fileReport);
                    fputs("</td></tr>\n", fileReport);
                    fputs("<tr><td bgcolor=\"#DAF7A6\">i_mtime</td><td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(inodo->i_mtime,fileReport);
                    fputs("</td></tr>\n", fileReport);
                    fputs("<tr><td bgcolor=\"#DAF7A6\">i_type</td><td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(&to_string(inodo->i_type)[0],fileReport);
                    fputs("</td></tr>\n", fileReport);
                    fputs("<tr><td bgcolor=\"#DAF7A6\">i_perm</td><td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(&to_string(inodo->i_perm)[0],fileReport);
                    fputs("</td></tr>\n", fileReport);
                    int i=0;
                      //APUNTADORES DIRECTOS
                    while(i<12){
                        fputs("<tr><td bgcolor=\"#FFC300\">AD", fileReport);
                        fputs(&to_string(i+1)[0],fileReport);
                        fputs("</td><td ",fileReport);
                        fputs(" bgcolor=\"#FFC300\">",fileReport);
                        fputs(&to_string(inodo->i_block[i])[0],fileReport);
                        fputs("</td></tr>\n", fileReport);
                        i++;
                    }

                    while (i<SIZE_BLOCKS_INODE) {
                      fputs("<tr><td bgcolor=\"#FF5733\">AI", fileReport);
                      fputs(&to_string(i-11)[0],fileReport);
                      fputs("</td><td bgcolor=\"#FF5733\">",fileReport);
                      fputs(&to_string(inodo->i_block[i])[0],fileReport);
                      fputs("</td></tr>\n", fileReport);
                    i++;
                    }

                    fputs("</table>\n",fileReport);
                    fputs(">];\n",fileReport);
                }
                //******
                contador++;
            }
         fputs("}\n",fileReport);
         fclose (fileReport);
         fclose (myFile);
         string pathString(path_report);
         string command = "dot -Tpng report_inodes.dot -o \""+pathString+"\"";//+"/report_mbr.png";
         system(command.c_str());
         return SUCCESS;
}

Response reportBlocks(char path[], char name[], char path_report[]){
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
      fileReport = fopen ("report_blocks.dot","w");
      if (fileReport==NULL)
      {
          cout<<"Error al crear archivo de reporte\n";
          return ERROR_UNHANDLED;
      }
      fseek(fileReport, 0, SEEK_SET);
      fputs("digraph di{\n", fileReport);
            //reporte de inodos
            char caracter;
            int contador = 0;
            Inodo *inodo;
             fseek(myFile, sb->s_bm_inode_start, SEEK_SET);
            while(contador<sb->s_inodes_count){
                fread(&caracter, sizeof(char), 1, myFile);
                if(caracter == '1'){
                    inodo = readInodo(path,getInitInode(sb,contador));
                    if(inodo == NULL){
                        break;
                    }
                    int i;
                    //apuntadores directos
                    for(i=0;i<12;i++){
                        if(inodo->i_block[i]!=-1){
                            if(inodo->i_type == IN_DIRECTORY){
                                BlockDirectory *block = readBlockDirectory(path,getInitBlock(sb,inodo->i_block[i]));
                                graphBlockDirectory(block,inodo->i_block[i],fileReport,contador,sb,path,false);
                                //******
                            }else if(inodo->i_type == IN_FILE){
                                BlockFile *block = readBlockFile(path,getInitBlock(sb,inodo->i_block[i]));
                                graphBlockFile(block,inodo->i_block[i],fileReport,contador);
                            }
                        }
                    }
                    //apuntadores indirectos
                    for(i=12;i<15;i++){
                        if(inodo->i_block[i]!=-1){
                            graphBlockPointer(i-11,contador,inodo->i_block[i],fileReport,path,sb,false,inodo->i_type);
                        }
                    }
                }
                contador++;
            }
         fputs("}\n",fileReport);
         fclose (fileReport);
         fclose (myFile);
         string pathString(path_report);
         string command = "dot -Tpng report_blocks.dot -o \""+pathString+"\"";//+"/report_mbr.png";
         system(command.c_str());
         return SUCCESS;
}

Response reportFile(char filePath[], char path[], char partition[], char reportPath[]){
    char *title="";
    string res = findContentFile(filePath,path,partition,&title);
    return graphFile(&res[0],title,reportPath);
}

Response graphFile(char *text, char *title,char reportPath[]){
    FILE * fileReport;
     fileReport = fopen ("report_file.dot","w+");
     if (fileReport==NULL)
     {
         cout<<"Error al crear archivo de reporte\n";
         return ERROR_UNHANDLED;
     }
     fseek(fileReport, 0, SEEK_SET);
     fputs("digraph di{\n", fileReport);
     fputs("nodo[label=\"",fileReport);
        int index = 0;
         while(text[index]!='\0' && text[index]>0){
            fprintf(fileReport,"%c", text[index]);
            if(index%64 == 0 && index!=0){
                fputs("\n",fileReport);
            }
            index++;
         }
     fputs("\"\n shape=\"note\"];\n",fileReport);
     fputs("labelloc=\"t\";\nlabel=\"",fileReport);
     fputs(title,fileReport);
     fputs("\";",fileReport);
     fputs("}\n",fileReport);
     fclose (fileReport);
     string pathString(reportPath);
     string command = "dot -Tpng report_file.dot -o \""+pathString+"\"";//+"/report_mbr.png";
     system(command.c_str());
     return SUCCESS;
}

Response reportJournal(char path[], char namePart[],char reportPath[]){
    int startSb;
    SuperBlock *sb = readSuperBlock(path,namePart,&startSb);
    if(sb==NULL){
        return ERROR_UNHANDLED;
    }

    FILE * fileReport;
     fileReport = fopen ("report_journal.dot","w+");
     if (fileReport==NULL)
     {
         cout<<"Error al crear archivo de reporte\n";
         return ERROR_UNHANDLED;
     }
     fseek(fileReport, 0, SEEK_SET);
     fputs("digraph di{\n", fileReport);
     fputs("i_3[ shape=plaintext label=<\n",fileReport);
     fputs("<table>\n",fileReport);
     fputs("<tr><td colspan=\"9\" bgcolor=\"#DAF7A6\">Journal</td></tr>\n",fileReport);
     fputs("<tr>\n",fileReport);
     fputs("<td bgcolor=\"#DAF7A6\">Operacion</td>\n",fileReport);
     fputs("<td bgcolor=\"#DAF7A6\">Fecha</td>\n",fileReport);
     fputs("<td bgcolor=\"#DAF7A6\">Ruta</td>\n",fileReport);
     fputs("<td bgcolor=\"#DAF7A6\">Contenido</td>\n",fileReport);
     fputs("<td bgcolor=\"#DAF7A6\">Tamaño</td>\n",fileReport);
     fputs("<td bgcolor=\"#DAF7A6\">Grupo</td>\n",fileReport);
     fputs("<td bgcolor=\"#DAF7A6\">Usuario</td>\n",fileReport);
     fputs("<td bgcolor=\"#DAF7A6\">Permisos</td>\n",fileReport);
     fputs("<td bgcolor=\"#DAF7A6\">Recursivo</td>\n",fileReport);
     fputs("</tr>\n",fileReport);
     int startOperations = startSb+sizeof(SuperBlock);
     FILE * myFile;
     int contador = 0;
      myFile = fopen (path,"rb+");
      if (myFile==NULL)
      {
          cout<<"Error al abrir el disco\n";
          return SUCCESS;
      }
      Journal *journal = (Journal*)malloc(sizeof(Journal));
        fseek(myFile, startOperations, SEEK_SET);
      while(contador<sb->s_inodes_count){
            fread(journal,sizeof(Journal),1,myFile);
            if(journal == NULL){
                return ERROR_UNHANDLED;
            }
                switch (journal->j_operation) {
                case MKDIRECTORY:
                {
                    fputs("<tr>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">Mkdir</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_date,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_path,fileReport);
                    fputs("</td>\n",fileReport);
                    //contenido
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    //tamaño
                    fputs("<td bgcolor=\"#DAF7A6\">-</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_group,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_user,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(&to_string(journal->j_perms)[0],fileReport);
                    fputs("</td>\n",fileReport);
                    if(journal->j_boolean){
                        fputs("<td bgcolor=\"#DAF7A6\">SI</td>\n",fileReport);

                    }else{
                        fputs("<td bgcolor=\"#DAF7A6\">NO</td>\n",fileReport);
                    }

                    fputs("</tr>\n",fileReport);
                }
                    break;
                case MKFILE_PATH:
                {
                    fputs("<tr>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">Mkfile</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_date,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_path,fileReport);
                    fputs("</td>\n",fileReport);
                    //contenido
                    fputs("<td bgcolor=\"#DAF7A6\"> ",fileReport);
                    fputs(journal->j_content,fileReport);
                    fputs(" </td>\n",fileReport);
                    //tamaño
                    fputs("<td bgcolor=\"#DAF7A6\">-</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_group,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_user,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(&to_string(journal->j_perms)[0],fileReport);
                    fputs("</td>\n",fileReport);
                    if(journal->j_boolean){
                        fputs("<td bgcolor=\"#DAF7A6\">SI</td>\n",fileReport);

                    }else{
                        fputs("<td bgcolor=\"#DAF7A6\">NO</td>\n",fileReport);
                    }



                    fputs("</tr>\n",fileReport);
                }
                    break;
                case MKFILE_SIZE:
                {
                    fputs("<tr>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">Mkfile</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_date,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_path,fileReport);
                    fputs("</td>\n",fileReport);
                    //contenido
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    //tamaño
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(&to_string(journal->j_size)[0],fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_group,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_user,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(&to_string(journal->j_perms)[0],fileReport);
                    fputs("</td>\n",fileReport);
                    if(journal->j_boolean){
                        fputs("<td bgcolor=\"#DAF7A6\">SI</td>\n",fileReport);

                    }else{
                        fputs("<td bgcolor=\"#DAF7A6\">NO</td>\n",fileReport);
                    }

                    fputs("</tr>\n",fileReport);
                }
                    break;
                case ADDGRP:
                {
                    fputs("<tr>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">mkgrp</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_date,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    //contenido
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    //tamaño
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_group,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("</tr>\n",fileReport);
                }
                    break;
                case DELGRP:
                {
                    fputs("<tr>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">rmgrp</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_date,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    //contenido
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    //tamaño
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_group,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("</tr>\n",fileReport);
                }
                    break;
                case DELUSER:
                {
                    fputs("<tr>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">rmusr</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_date,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    //contenido
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    //tamaño
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_user,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("</tr>\n",fileReport);
                }
                    break;
                case ADDUSER:
                {
                    fputs("<tr>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">mkusr</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_date,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    //contenido
                    fputs("<td bgcolor=\"#DAF7A6\"> ",fileReport);
                    fputs(journal->j_content,fileReport);
                    fputs(" </td>\n",fileReport);
                    //tamaño
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> ",fileReport);
                    fputs(journal->j_group,fileReport);
                    fputs(" </td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> ",fileReport);
                    fputs(journal->j_user,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("</tr>\n",fileReport);
                }
                    break;
                case EDIT_FILE:
                {
                    fputs("<tr>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">mkusr</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\">",fileReport);
                    fputs(journal->j_date,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> ",fileReport);
                    fputs(journal->j_path,fileReport);
                    fputs(" </td>\n",fileReport);
                    //contenido
                    fputs("<td bgcolor=\"#DAF7A6\"> ",fileReport);
                    fputs(journal->j_content,fileReport);
                    fputs(" </td>\n",fileReport);
                    //tamaño
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> ",fileReport);
                    fputs(journal->j_group,fileReport);
                    fputs(" </td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> ",fileReport);
                    fputs(journal->j_user,fileReport);
                    fputs("</td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("<td bgcolor=\"#DAF7A6\"> - </td>\n",fileReport);
                    fputs("</tr>\n",fileReport);
                }
                    break;
                default:
                    contador = sb->s_inodes_count+1;
                    break;
            }
          contador++;
      }

      fclose (myFile);


     fputs("</table>\n",fileReport);
     fputs(">];\n",fileReport);
     fputs("}\n",fileReport);
     fclose (fileReport);
     string pathString(reportPath);
     string command = "dot -Tpng report_journal.dot -o \""+pathString+"\"";//+"/report_mbr.png";
     system(command.c_str());
     return SUCCESS;
}

Response reportLs(char path[], char name[], char path_report[]){
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
      fileReport = fopen ("report_ls.dot","w+");
      if (fileReport==NULL)
      {
          cout<<"Error al crear archivo de reporte\n";
          return ERROR_UNHANDLED;
      }
      fseek(fileReport, 0, SEEK_SET);
      fputs("digraph d{\n", fileReport);
      fputs("i_3[ shape=plaintext label=<\n", fileReport);
      fputs("<table>\n", fileReport);

      fputs("<tr>\n", fileReport);
      fputs("<td bgcolor=\"#f99c87\">Permisos</td>\n", fileReport);
      fputs("<td bgcolor=\"#f99c87\">Usuario Propietario</td>\n", fileReport);
      fputs("<td bgcolor=\"#f99c87\">Grupo</td>\n", fileReport);
      fputs("<td bgcolor=\"#f99c87\">Tamaño</td>\n", fileReport);
      fputs("<td bgcolor=\"#f99c87\">Fecha y Hora</td>\n", fileReport);
      fputs("<td bgcolor=\"#f99c87\">Tipo</td>\n", fileReport);
      fputs("<td bgcolor=\"#f99c87\">Nombre</td>\n", fileReport);
      fputs("</tr>\n", fileReport);
      Inodo *inodo = readInodo(path,getInitInode(sb,0));
      if(inodo!=NULL){
          graphInodoForLs(inodo,fileReport,path,name,sb,"/");
      }
      fputs("</table>\n>];\n", fileReport);
      fputs("}\n",fileReport);
      fclose (fileReport);
      fclose (myFile);
      string pathString(path_report);
      string command = "dot -Tpng report_ls.dot -o \""+pathString+"\"";//+"/report_mbr.png";
      system(command.c_str());
      return SUCCESS;
}

void graphInodoForLs(Inodo* inodo,FILE *fileReport,char path[],char namePart[],SuperBlock *sb,char name[]){
    if(inodo==NULL) return;
        if(inodo->i_type == IN_DIRECTORY){
            fputs("<tr>\n", fileReport);
            fputs("<td bgcolor=\"#f99c87\">", fileReport);
            //permisos
            fputs(&to_string(inodo->i_perm)[0],fileReport);
            fputs("</td>\n", fileReport);
            fputs("<td bgcolor=\"#f99c87\">", fileReport);
            //usuario
            User *user = getUserById(&to_string(inodo->i_uid)[0],path,namePart);
            if(user!=NULL){
                fputs(&user->name[0], fileReport);
            }
            fputs("</td>\n", fileReport);
            fputs("<td bgcolor=\"#f99c87\">", fileReport);
            //grupo
            Group *grupo = getGroupById(&to_string(inodo->i_gid)[0],path,namePart);
            if(grupo!=NULL){
                fputs(&grupo->name[0], fileReport);
            }
            fputs("</td>\n", fileReport);
            fputs("<td bgcolor=\"#f99c87\">", fileReport);
            //tamaño
            fputs(&to_string(inodo->i_size)[0],fileReport);
            fputs("</td>\n", fileReport);
            fputs("<td bgcolor=\"#f99c87\">", fileReport);
            //fecha y hora
            fputs(inodo->i_ctime,fileReport);
            fputs("</td>\n", fileReport);
            fputs("<td bgcolor=\"#f99c87\">Carpeta</td>\n", fileReport);
            fputs("<td bgcolor=\"#f99c87\">", fileReport);
            //nombre
               fputs(name, fileReport);
            fputs("</td>\n", fileReport);
            fputs("</tr>\n", fileReport);

            int indexInodo;
            for(indexInodo = 0;indexInodo<12;indexInodo++){
                if(inodo->i_block[indexInodo]!=-1){
                    if(inodo->i_type == IN_DIRECTORY){
                        BlockDirectory *directory = readBlockDirectory(path,getInitBlock(sb,inodo->i_block[indexInodo]));
                        if(directory!=NULL){
                            int indexBlock;
                            for(indexBlock = 2;indexBlock<4;indexBlock++){
                                if(directory->b_content[indexBlock].b_inodo!=-1){
                                    Inodo *inodo2 = readInodo(path,getInitInode(sb,directory->b_content[indexBlock].b_inodo));
                                    if(inodo2!=NULL){
                                        graphInodoForLs(inodo2,fileReport,path,namePart,sb,directory->b_content[indexBlock].b_name);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            //GRAFICAR INDIRECTOS
            while(indexInodo<SIZE_BLOCKS_INODE){
                if(inodo->i_block[indexInodo]!=-1){
                    graphInodoForLsRec(indexInodo-11,inodo->i_block[indexInodo],fileReport,path,namePart,sb,inodo->i_type);
                }
                indexInodo++;
            }
        }else{
            fputs("<tr>\n", fileReport);
            fputs("<td bgcolor=\"#f99c87\">", fileReport);
            //permisos
            fputs(&to_string(inodo->i_perm)[0],fileReport);
            fputs("</td>\n", fileReport);
            fputs("<td bgcolor=\"#f99c87\">", fileReport);
            //usuario
            User *user = getUserById(&to_string(inodo->i_uid)[0],path,namePart);
            if(user!=NULL){
                fputs(&user->name[0], fileReport);
            }
            fputs("</td>\n", fileReport);
            fputs("<td bgcolor=\"#f99c87\">", fileReport);
            //grupo
            Group *grupo = getGroupById(&to_string(inodo->i_gid)[0],path,namePart);
            if(grupo!=NULL){
                fputs(&grupo->name[0], fileReport);
            }
            fputs("</td>\n", fileReport);fputs("<td bgcolor=\"#f99c87\">", fileReport);
            //tamaño
            fputs(&to_string(inodo->i_size)[0],fileReport);
            fputs("</td>\n", fileReport);
            fputs("<td bgcolor=\"#f99c87\">", fileReport);
            //fecha y hora
            fputs(inodo->i_ctime,fileReport);
            fputs("</td>\n", fileReport);
            fputs("<td bgcolor=\"#f99c87\">Archivo</td>\n", fileReport);
            fputs("<td bgcolor=\"#f99c87\">", fileReport);
            //nombre
            fputs(name, fileReport);
            fputs("</td>\n", fileReport);
            fputs("</tr>\n", fileReport);
        }
}

void graphInodoForLsRec(int level,int indexBlock,FILE *fileReport,char path[],char namePart[],SuperBlock *sb,TypeInode type){
    BlockPointer *block = readBlockPointer(path,getInitBlock(sb,indexBlock));
    if(block==NULL) return;

    if(level==1){
        int indexPointer;
        for(indexPointer = 0;indexPointer<15;indexPointer++){
            if(block->b_pointers[indexPointer]!=-1){
                if(type==IN_DIRECTORY){
                    Inodo *ind = readInodo(path,getInitInode(sb,block->b_pointers[indexPointer]));
                    BlockDirectory *directory = readBlockDirectory(path,getInitBlock(sb,ind->i_block[0]));
                    if(directory!=NULL){

                        graphInodoForLs(ind,fileReport,path,namePart,sb,directory->b_content[0].b_name);
                    }
                }
            }
        }
    }else{
           int indexPointer;
           for(indexPointer = 0;indexPointer<15;indexPointer++){
               graphInodoForLsRec(level-1,block->b_pointers[indexPointer],fileReport,path,namePart,sb,type);
           }
        }
}
