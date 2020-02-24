#include "handlerdisk.h"

void newDisk(int size,Fit fit,Unit unit,char path[],char name[]){

     //VALIDAR TAMAÑO
     long final_size = getSize(size,unit);
     final_size-=sizeof(MBR);
    //MBR
    MBR* disco = (MBR*)malloc(sizeof(MBR));
    getCurrentDate(disco->mbr_fecha_creacion);
    disco->disk_fit = fit;
    disco->mbr_disk_signature=getSignature();
    disco->mbr_tamanio = final_size;
    //PARTITIONS
    for(int i = 0; i < 4; i++){
            disco->particiones[i].part_fit = fit;
            string nombre = "partition_"+to_string(i+1);
            clearArray(disco->particiones[i].part_name,16);
            strcpy(disco->particiones[i].part_name,nombre.c_str());
            disco->particiones[i].part_size = 0;
            disco->particiones[i].part_start = 0;
            disco->particiones[i].part_status = Inactivo;
            disco->particiones[i].part_type = Primaria;
        }
    //CREAR FULL PATH
    char full_path[200];
    clearArray(full_path,sizeof(full_path));
    getFullPathDisk(path,name,full_path);
    //ESCRIBIR ARCHIVO
    writeMBR(disco,path,full_path);
    //ESCRIBIR RAID
    char nameMirror[50];
    clearArray(nameMirror,50);
    strcat(nameMirror,name);
    strcat(nameMirror,"_mirror");
    clearArray(full_path,sizeof(full_path));
    getFullPathDisk(path,nameMirror,full_path);
    writeMBR(disco,path,full_path);
    fillSpaceWithZeros(full_path,sizeof(MBR),final_size);
    cout<<"Disco \'"<<name<<"\' creado con éxito\n";
}

void writeMBR(MBR *disco,char path[],char full_path[]){
    //CREAR DIRECTORIO SI NO EXISTE
    mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    FILE * myFile;
     myFile = fopen (full_path,"wb+");
     if (myFile==NULL)
     {
         cout<<"Error al crear el archivo\n";
         return;
     }
     //inicializando con ceros
     fseek(myFile, disco->mbr_tamanio, SEEK_SET);
     fwrite("\0", sizeof(char), 1, myFile);
     //escribir MBR en disco
     fseek(myFile, 0, SEEK_SET);
     fwrite(disco, sizeof(MBR), 1, myFile);
     //cerrando stream
     fclose (myFile);
}

int getSignature(){
    srand(time(NULL));
    int num = rand();
    num = 1 + rand() % (100000 - 1000000);
    return num;
}

void getCurrentDate(char *date){
    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    strftime(date, 16, "%x %H:%M", tlocal);
}

void deleteDisk(char path[]){
    cout<<"Está seguro que desea eliminar el disco? \n"
       <<"Presione S para eliminar o N para cancelar\n";
    char response;
    cin>>response;
     if(response == 83 || response == 115){
         if(canDeleteDisk(path)){
             if(remove(path) != 0 )
                 cout<<"Error al borrar el disco\n";
               else
                 cout<<"¡El disco se borro con exito!\n";
         }else{
              cout<<"El disco no se puede eliminar por qué tiene particiones montadas\n";
         }
     }else if(response!=78 && response!=110){
         cout<<"Tecla inválida\n";
     }
}

void reportMBR(char path[],char path_report[]){

    MBR* disco = openMBR(path);
    if(disco==NULL){
        cout<<"Error al generar reporte de MBR\n";
        return;
    }

    FILE * myFile;
     myFile = fopen ("report_mbr.dot","w+");
     if (myFile==NULL)
     {
         cout<<"Error al crear el archivo\n";
         return;
     }
     fseek(myFile, 0, SEEK_SET);
     fputs("digraph {\ntbl [\nshape=plaintext\n label=<\n", myFile);
     fputs("<table border='0' cellborder='1' cellspacing='0'>\n",myFile);
     fputs("<tr><td colspan=\"3\">Disk1</td></tr>\n",myFile);
     fputs("<th><td>Nombre</td><td>Valor</td></th>\n",myFile);
     //DATA OF MBR
     //TAMAÑO
     fputs("<tr><td bgcolor=\"lightblue\">mbr_tamaño</td><td bgcolor=\"lightblue\">",myFile);
     fputs(&to_string(disco->mbr_tamanio)[0],myFile);
     fputs("</td></tr>\n",myFile);
     //FECHA
     fputs("<tr><td bgcolor=\"lightblue\">mbr_Fecha_creación</td><td bgcolor=\"lightblue\">",myFile);
     fputs(disco->mbr_fecha_creacion,myFile);
     fputs("</td></tr>\n",myFile);
     //SIGNATURE
     fputs("<tr><td bgcolor=\"lightblue\">mbr_disk_signature</td><td bgcolor=\"lightblue\">",myFile);
     fprintf(myFile, "%d", disco->mbr_disk_signature);
     fputs("</td></tr>\n",myFile);
     //FIT
     fputs("<tr><td bgcolor=\"lightblue\">mbr_disk_fit</td><td bgcolor=\"lightblue\">",myFile);
     fprintf(myFile, "%c", disco->disk_fit);
     fputs("</td></tr>\n",myFile);

     int i;
     Partition part;
     char colors[4][10];
     clearArray(colors[0],10);
     strcat(colors[0],"#bcf7c1");
     clearArray(colors[1],10);
     strcat(colors[1],"#f8fc92");
     clearArray(colors[2],10);
     strcat(colors[2],"#fcc292");
     clearArray(colors[3],10);
     strcat(colors[3],"#dfbcf7");


     for(i=0;i<4;i++){
        part = disco->particiones[i];
        if(part.part_status == Inactivo) continue;
        //PART NAME
        fputs("<tr><td colspan=\"2\" bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\">",myFile);
        fputs(part.part_name,myFile);
        fputs("</td></tr>\n",myFile);
        //PART STATUS
        fputs("<tr><td bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\">part_status</td><td bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\">",myFile);
        fprintf(myFile, "%d", part.part_status);
        fputs("</td></tr>\n",myFile);
        //PART TYPE
        fputs("<tr><td bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\">part_type</td><td bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\">",myFile);
        fprintf(myFile, "%c", part.part_type);
        fputs("</td></tr>\n",myFile);
        //PART FIT
        fputs("<tr><td bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\">part_fit</td><td bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\">",myFile);
        fprintf(myFile, "%c", part.part_fit);
        fputs("</td></tr>\n",myFile);
        //PART START
        fputs("<tr><td bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\">part_start</td><td bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\">",myFile);
        fprintf(myFile, "%d", part.part_start);
        fputs("</td></tr>\n",myFile);
        //PART SIZE
        fputs("<tr><td bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\">part_size</td><td bgcolor=\"",myFile);
        fputs(colors[i],myFile);
        fputs("\">",myFile);
        fputs(&to_string(part.part_size)[0],myFile);
        fputs("</td></tr>\n",myFile);
     }

     fputs("</table>\n",myFile);
     fputs(">];\n\n", myFile);

     //REPORTE DE EBR'S

     i = 0;
     EBR *ebr = getFirstEBR(disco,path);

     while(ebr!=NULL){//existenExtendidas
        addReportEBR(ebr,myFile,i);
        if(ebr->part_next!=-1){
            ebr = readEBR(ebr->part_next,path);
        }else{
            ebr = NULL;
        }
        i++;
     }

     fputs("}\n",myFile);
     //cerrando stream
     fclose (myFile);
     string pathString(path_report);
     string command = "dot -Tpng report_mbr.dot -o \""+pathString+"\"";//+"/report_mbr.png";
     system(command.c_str());
     cout<<"Reporte de MBR creado...\n";
}

void addReportEBR(EBR *ebr,FILE *myFile,int index){
    string nombreNodo = "tbl"+to_string(index+1);
    fputs(nombreNodo.c_str(),myFile);
    fputs(" [\nshape=plaintext\n label=<\n", myFile);
    fputs("<table border='0' cellborder='1' cellspacing='0'>\n",myFile);
    fputs("<tr><td colspan=\"3\">",myFile);
    fputs(ebr->part_name,myFile);
    fputs("</td></tr>\n",myFile);
    fputs("<th><td>Nombre</td><td>Valor</td></th>\n",myFile);
    //PART STATUS
    fputs("<tr><td bgcolor=\"#fcc8c8\">part_status</td><td bgcolor=\"#fcc8c8\">",myFile);
    fprintf(myFile, "%d", ebr->part_status);
    fputs("</td></tr>\n",myFile);
    //PART FIT
    fputs("<tr><td bgcolor=\"#fcc8c8\">part_fit</td><td bgcolor=\"#fcc8c8\">",myFile);
    fprintf(myFile, "%c", ebr->part_fit);
    fputs("</td></tr>\n",myFile);
    //PART START
    fputs("<tr><td bgcolor=\"#fcc8c8\">part_start</td><td bgcolor=\"#fcc8c8\">",myFile);
    fprintf(myFile, "%d", ebr->part_start);
    fputs("</td></tr>\n",myFile);
    //PART SIZE
    fputs("<tr><td bgcolor=\"#fcc8c8\">part_size</td><td bgcolor=\"#fcc8c8\">",myFile);
    fputs(&to_string(ebr->part_size)[0],myFile);
    fputs("</td></tr>\n",myFile);
    //PART NEXT
    fputs("<tr><td bgcolor=\"#fcc8c8\">part_next</td><td bgcolor=\"#fcc8c8\">",myFile);
    fprintf(myFile, "%d", ebr->part_next);
    fputs("</td></tr>\n",myFile);
    fputs("</table>\n",myFile);
    fputs(">];\n", myFile);
}

MBR* openMBR(char path[]){
    FILE *myFile;
    myFile = fopen(path,"rb+");
    if(myFile==NULL){
        cout<<"Error: No existe el disco\n";
        return NULL;
    }
    fseek(myFile, 0, SEEK_SET);
    MBR *mbr = (MBR*)malloc(sizeof(MBR));
    fread(mbr,sizeof(MBR),1,myFile);
    fclose(myFile);
return mbr;
}

void replaceMBR(MBR *disco,char path[]){
    FILE * myFile;
     myFile = fopen (path,"rb+");
     if (myFile==NULL)
     {
         cout<<"Error al abrir el archivo\n";
         return;
     }
     //escribir MBR en disco
     fseek(myFile, 0, SEEK_SET);
     fwrite(disco, sizeof(MBR), 1, myFile);
     //cerrando stream
     fclose (myFile);
}

EBR* getFirstEBR(MBR *disco,char path[]){
    int i;
    Partition *extended = NULL;
    for(i=0;i<4;i++){
        if(disco->particiones[i].part_status == Activo && disco->particiones[i].part_type == Extendida){
            extended = &disco->particiones[i];
            break;
        }
    }
        if(extended!=NULL){
            return readEBR(extended->part_start,path);
        }

    return NULL;
}

EBR* readEBR(int point, char path[]){
    FILE *myFile = fopen(path,"rb+");
    if(myFile==NULL){
        cout<<"Error al abrir el archivo \n";
        return NULL;
    }
    EBR *ebr = (EBR*)malloc(sizeof(EBR));

    fseek(myFile, point, SEEK_SET);
    fread(ebr, sizeof(EBR), 1, myFile);
    fclose(myFile);
    return ebr;
}

Partition* getExtendedPart(char name[], MBR *disco){
    int i;
    for(i=0;i<4;i++){
        if(disco->particiones[i].part_status == Activo && disco->particiones[i].part_type == Extendida){
            if(strcmp(disco->particiones[i].part_name,name)==0){
                return &disco->particiones[i];
            }

        }
    }
    return NULL;
}

void reportDisk(char path[], char path_report[]){
    MBR* disco = openMBR(path);
    if(disco==NULL){
        cout<<"Error al generar reporte de disco\n";
        return;
    }

    FILE * myFile;
     myFile = fopen ("report_disk.dot","w+");
     if (myFile==NULL)
     {
         cout<<"Error al crear el archivo de reporte\n";
         return;
     }
    fseek(myFile, 0, SEEK_SET);
    fputs("digraph G {\n", myFile);
    fputs("parent [\n", myFile);
    fputs("shape=plaintext\n", myFile);
    fputs("label=<\n", myFile);
    fputs("<table border='1' cellborder='1'>\n", myFile);
    fputs("<tr>\n", myFile);

    //MBR
    fputs("<td rowspan=\"2\" bgcolor =\"#dd8703\" >MBR</td>\n", myFile);

    int i;
    int inicio = sizeof(MBR);
    int fin = sizeof(MBR)-1;

    for(i=0;i<4;i++){
        if(disco->particiones[i].part_status == Activo){
        switch(disco->particiones[i].part_type){
        case Primaria:
            fin = disco->particiones[i].part_start;
            if(fin-inicio > 0){
                //ESPACIO LIBRE
                fputs("<td rowspan=\"2\" bgcolor = \"#3ac9da\">Libre<br/>\n", myFile);
                fprintf(myFile, "%.2f\n",(getDecimal((float)((fin-inicio)*100)/disco->mbr_tamanio)));
                fputs("%</td>\n", myFile);
            }
            inicio = disco->particiones[i].part_start+disco->particiones[i].part_size;
            //PARTICION PRIMARIA
            fputs("<td rowspan=\"2\" bgcolor =\"#50b104\" >Primaria <br/>",myFile);
            fprintf(myFile, "%.2f\n",getDecimal((float)(disco->particiones[i].part_size*100)/disco->mbr_tamanio));
            fputs("%</td>\n", myFile);
            break;
        case Extendida:
            fin = disco->particiones[i].part_start;
            if(fin-inicio > 0){
                //ESPACIO LIBRE
                fputs("<td rowspan=\"2\" bgcolor = \"#3ac9da\">Libre<br/>\n", myFile);
                fprintf(myFile, "%.2f\n",getDecimal((float)((fin-inicio)*100)/disco->mbr_tamanio));
                fputs("%</td>\n", myFile);
            }
            inicio = disco->particiones[i].part_start+disco->particiones[i].part_size;
            //PARTICION EXTENDIDA
            fputs("<td>\n", myFile);
            writeExtendedReport(myFile,disco,path);
            fputs("</td>\n", myFile);
            break;
        }
       }
    }
    if(disco->mbr_tamanio-inicio > 0){
        //ESPACIO LIBRE
        fputs("<td rowspan=\"2\" bgcolor = \"#3ac9da\">Libre<br/>\n", myFile);
        fprintf(myFile, "%.2f\n", getDecimal((float)((disco->mbr_tamanio-inicio)*100)/disco->mbr_tamanio));

        fputs("%</td>\n", myFile);
    }

    fputs("</tr>\n", myFile);
    fputs("</table>\n", myFile);
    fputs(">];\n", myFile);

     fputs("}\n",myFile);
     //cerrando stream
     fclose (myFile);
     string pathString(path_report);
     string command = "dot -Tpng report_disk.dot -o \2"+pathString+"\"";//+"/report_disk.png";
     system(command.c_str());
     cout<<"Reporte de disco creado...\n";
}

void writeExtendedReport(FILE *myFile, MBR *disco, char path[]){

    EBR *first = getFirstEBR(disco,path);
    if(first == NULL){
        return;
    }
    int i;
    Partition *extended = NULL;
    for(i=0;i<4;i++){
        if(disco->particiones[i].part_status == Activo && disco->particiones[i].part_type == Extendida){
            extended = &disco->particiones[i];
            break;
        }
    }
        if(extended==NULL){
            return;
        }
    fputs("<table border = \"1\" cellborder=\"1\" bgcolor=\"#da3a85\">\n", myFile);
    //fputs("<tr><td colspan=\"3\">Extendida</td></tr>\n", myFile);
    fputs("<tr>\n", myFile);
    bool flag = true;
    int inicio = extended->part_start;
    int fin = 0;
    while(flag){
        fin = first->part_start-sizeof(EBR);
        if(fin-inicio > 0){
            //ESPACIO LIBRE
            fputs("<td rowspan=\"2\" bgcolor = \"#3ac9da\">\n", myFile);
            fprintf(myFile, "%.2f\n",getDecimal((float)((fin-inicio)*100)/disco->mbr_tamanio));
            fputs("%</td>\n", myFile);
        }
        inicio = first->part_start+first->part_size;
        if(first->part_status == Activo){
            fputs("<td bgcolor=\"#eeeeee\">EBR</td>\n", myFile);
        }
        fputs("<td bgcolor=\"#eeeeee\">", myFile);
        if(first->part_status == Inactivo){
            fputs("Libre <br/>",myFile);
        }else{
            fputs("Lógica <br/>",myFile);
        }
        fprintf(myFile, "%.2f\n",getDecimal((float)(first->part_size*100)/disco->mbr_tamanio));
        fputs("%</td>\n",myFile);
        if(first->part_next!=-1){
           first = readEBR(first->part_next,path);
       }else{
           flag = false;
       }
    }
    if(disco->mbr_tamanio-inicio > 0){
        //ESPACIO LIBRE
        fputs("<td rowspan=\"2\" bgcolor = \"#3ac9da\">Libre<br/>\n", myFile);
        fprintf(myFile, "%.2f\n",getDecimal((float)(((extended->part_start+extended->part_size)-inicio)*100)/disco->mbr_tamanio));
        fputs("%</td>\n", myFile);
    }
    fputs("</tr>\n", myFile);
    fputs("</table>\n", myFile);
}

bool canDeleteDisk(char path[]){
    int contadorDiscos = 0;
    bool exist= false;
    //MONTAR DISCO
    while(partsMounted[contadorDiscos]!=NULL){
        if(strcmp(partsMounted[contadorDiscos]->path,path)==0){
            exist = true;
            break;
        }
        contadorDiscos++;
    }

    return !exist;
}
