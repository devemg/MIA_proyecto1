#include "utils.h"

void clearArray(char array[],int size){
        for(int i = 0; i < size; i++)
        {
            array[i] = 0;
        }
}

void getFullPathDisk(char path[],char name[],char full_path[]){
    strcat(full_path,path);
    strcat(full_path,"/");
    strcat(full_path,name);
    strcat(full_path,".disk");
}

void getFullPath(char path[],char name[],char full_path[]){
    strcat(full_path,path);
    strcat(full_path,"/");
    strcat(full_path,name);
}

int getSize(int size,Unit unit){
    switch(unit)
    {
       case Byte:
            return size;
       case KB: //equivale a kilobytes
           return size*1024;
           break;
       case MB: //equivale a MegaBytes
           return size * 1024 * 1024;
           break;
       default:
           return size * 1024 * 1024;
           break;
  }

}

void showMessageError(Response response){
    switch (response) {
    case ERROR_FULL_PARTITION_PRIMARY:
        cout<<"Error al crear la partición. Ya existen cuatro particiones primarias creadas\n";
        break;
    case ERROR_INSUFICIENT_SPACE:
        cout<<"Error al crear la partición. No hay espacio suficiente\n";
        break;
    case ERROR_EXISTS_EXTEND_PARTITION:
        cout<<"Ya existe una partición extendida en el disco\n";
        break;
    }
}

void fillSpaceWithZeros(char full_path[],int position,int size){
    FILE * myFile;
     myFile = fopen (full_path,"rb+");
     if (myFile==NULL)
     {
         cout<<"Error al abrir el archivo\n";
         return;
     }
    fseek(myFile,position , SEEK_SET);
    fwrite("\0", sizeof(char), size, myFile);
}
