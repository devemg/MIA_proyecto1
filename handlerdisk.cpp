#include "handlerdisk.h"

void newDisk(int size,Fit fit,Unit unit,char path[],char name[]){

     //VALIDAR TAMAÑO
     int final_size = 0;
      switch(unit)
      {
         case KB: //equivale a kilobytes
             final_size = size*1024;
             break;
         case MB: //equivale a MegaBytes
             final_size = size * 1024 * 1024;
             break;
         default:
             final_size = size * 1024 * 1024;
             break;
    }
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
            clearArray(disco->particiones[i].part_name, 16);
            strcpy(disco->particiones[i].part_name, "partition_1");
            disco->particiones[i].part_size = 0;
            disco->particiones[i].part_start = 0;
            disco->particiones[i].part_status = 0;
            disco->particiones[i].part_type = 0;

        }

    //CREAR DIRECTORIO SI NO EXISTE
    mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    //CREAR FULL PATH
    char full_path[200];
    clearArray(full_path,sizeof(full_path));
    getFullPath(path,name,full_path);
    //CREAR ARCHIVO****************************************************
    FILE * myFile;
     myFile = fopen (full_path,"wb+");
     if (myFile==NULL)
     {
         cout<<"Error al crear el archivo\n";
         return;
     }
     //inicializando con ceros
     fseek(myFile, final_size, SEEK_SET);
     fwrite("\0", sizeof(char), 1, myFile);
     //escribir MBR en disco
     fseek(myFile, 0, SEEK_SET);
     fwrite(disco, sizeof(MBR), 1, myFile);
     //cerrando stream
     fclose (myFile);

    cout<<"Disco \'"<<name<<"\' creado con éxito\n";
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

