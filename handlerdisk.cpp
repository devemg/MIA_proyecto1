#include "handlerdisk.h"

void newDisk(int size,Fit fit,Unit unit,char path[],char name[]){

     //VALIDAR TAMAÑO
     int final_size = getSize(size,unit);
    //MBR
    MBR* disco = (MBR*)malloc(sizeof(MBR));
    getCurrentDate(disco->mbr_fecha_creacion);
    disco->disk_fit = fit;
    disco->mbr_disk_signature=getSignature();
    disco->mbr_tamanio = final_size;
    //PARTITIONS
    for(int i = 0; i < 4; i++)
        {
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
         if(remove(path) != 0 )
             cout<<"Error al borrar el disco\n";
           else
             cout<<"¡El disco se borro con exito!\n";
     }else if(response!=78 && response!=110){
         cout<<"Tecla inválida\n";
     }
}

void reportMBR(char path[],char path_report[]){

    MBR* disco = openMBR(path);
    if(disco==NULL){
        cout<<"Error al generar reporte\n";
        return;
    }

    FILE * myFile;
     myFile = fopen ("report_disk.dot","w+");
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
     fprintf(myFile, "%d", disco->mbr_tamanio);
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
        fprintf(myFile, "%d", part.part_size);
        fputs("</td></tr>\n",myFile);
     }

     fputs("</table>\n",myFile);
     fputs(">];\n\n", myFile);

     //REPORTE DE EBR'S

     i = 0;
     EBR *ebr = getFirstEBR(disco,path);

     if(ebr!=NULL){//existenExtendidas
        addReportEBR(ebr,myFile,i);
     }

     fputs("}\n",myFile);
     //cerrando stream
     fclose (myFile);
     string pathString(path_report);
     string command = "dot -Tpng report_disk.dot -o "+pathString;//+"/report_disk.png";
     system(command.c_str());
     cout<<"Reporte de disco creado...\n";
}

void addReportEBR(EBR *ebr,FILE *myFile,int index){
    string nombreNodo;
    nombreNodo = "tbl"+to_string(index+1);
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
    fprintf(myFile, "%d", ebr->part_size);
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
        cout<<"Error al abrir el disco\n";
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
            FILE *myFile = fopen(path,"rb+");
            if(myFile==NULL){
                cout<<"Error al abrir el archivo \n";
                return NULL;
            }
            EBR *ebr = (EBR*)malloc(sizeof(EBR));

            fseek(myFile, extended->part_start, SEEK_SET);
            fread(ebr, sizeof(EBR), 1, myFile);
            fclose(myFile);
            return ebr;
        }

    return NULL;
}
