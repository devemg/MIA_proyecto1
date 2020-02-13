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
