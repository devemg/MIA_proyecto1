#include "handlerpartitions.h"

void createPartition(int size, Unit unit, char path[],char nameDisk[],TipoParticion tipoParticion, Fit fit, char name[]){
    char full_path[200];
    clearArray(full_path,sizeof(full_path));
    getFullPathDisk(path,nameDisk,full_path);
    //ESCRIBIR ARCHIVO
    Response response = newPartition(size,unit,full_path,tipoParticion,fit,name);
    if(response == SUCCESS){
        char nameMirror[50];
        clearArray(nameMirror,50);
        strcat(nameMirror,nameDisk);
        strcat(nameMirror,"_mirror");
        clearArray(full_path,sizeof(full_path));
        getFullPathDisk(path,nameMirror,full_path);
        //ESCRIBIR RAID
        //Response response1 =
        newPartition(size,unit,full_path,tipoParticion,fit,name);
        switch (tipoParticion) {
        case Primaria:
            cout<<"¡La partición primaria fue creada con éxito!\n";
            break;
        case Extendida:
            cout<<"¡La partición extendida fue creada con éxito!\n";
            break;
        case Logica:
            cout<<"¡La partición lógica fue creada con éxito!\n";
            break;
        }
        //if(response1 != SUCCESS)cout<<"Error al hacer cambio en el raid\n";
    }else{
        showMessageError(response);
    }

}

Response newPartition(int size, Unit unit, char path[], TipoParticion tipoParticion, Fit fit, char name[]){
    MBR *disco = openMBR(path);
    if(disco==NULL){
        return ERROR_UNHANDLED;
    }
    int final_size = getSize(size,unit);
    switch (tipoParticion) {
    case Primaria:
        return newPrimaryPart(final_size,fit,name,disco,path);
   case Extendida:
        return newExtendedPart(final_size,fit,name,disco,path);
    case Logica:
        return newLogicPart(final_size,fit,name,disco,path);
        break;
    }
}

Response newPrimaryPart(int size,Fit fit,char name[],MBR *disco,char path[]){
    int i;
    Partition *partition;
    bool hayEspacio=false;
    int espacioOcupado=0;
    //BUSCAR PARTICION
    for(i=0;i<4;i++){
        partition  = &disco->particiones[i];
        if(partition->part_status==Inactivo){
            hayEspacio = true;
            break;
        }else{
            espacioOcupado+=partition->part_size;
        }
    }
    if(hayEspacio){
        if(size>(disco->mbr_tamanio-espacioOcupado)){
            return ERROR_INSUFICIENT_SPACE;
        }
            partition  = &disco->particiones[i];
                partition->part_fit = fit;
                strcpy(partition->part_name,name);
                partition->part_size = size;
                partition->part_start=(espacioOcupado+sizeof(MBR));
                partition->part_type = Primaria;
                partition->part_status = Activo;
                replaceMBR(disco,path);
                //fillSpaceWithZeros(path,partition->part_start,size);
                return SUCCESS;
    }else{
        return ERROR_FULL_PARTITION_PRIMARY;
    }
}

Response newExtendedPart(int size, Fit fit, char name[], MBR *disco, char path[]){
    int i;
    Partition *partition;
    bool hayEspacio=false;
    bool noHayPartExt = true;
    int espacioOcupado=0;
    //BUSCAR PARTICION
    for(i=0;i<4;i++){
        partition  = &disco->particiones[i];
        if(partition->part_type == Extendida){
            noHayPartExt = false;
            break;
        }
    }
    if(noHayPartExt)
    for(i=0;i<4;i++){
        partition  = &disco->particiones[i];
        if(partition->part_status==Inactivo){
            hayEspacio = true;
            break;
        }else{
            espacioOcupado+=partition->part_size;
        }
    }

        if(noHayPartExt){
            if(hayEspacio){
                if(size>(disco->mbr_tamanio-espacioOcupado)){
                    return ERROR_INSUFICIENT_SPACE;
                }
                partition  = &disco->particiones[i];
                    partition->part_fit = fit;
                    strcpy(partition->part_name,name);
                    partition->part_size = size;
                    partition->part_start=(espacioOcupado+sizeof(MBR));
                    partition->part_type = Extendida;
                    partition->part_status = Activo;
                    replaceMBR(disco,path);
                    newEBR(partition,path);
                    return SUCCESS;
            }else{
                return ERROR_FULL_PARTITION_PRIMARY;
            }
        }else{
            return ERROR_EXISTS_EXTENDED_PARTITION;
        }

}

void newEBR(Partition *partition, char path[]){
    EBR *extendedPart = (EBR*)malloc(sizeof(EBR));
    extendedPart->part_fit = partition->part_fit;
    strcpy(extendedPart->part_name,partition->part_name);
    extendedPart->part_next = -1;
    extendedPart->part_size = partition->part_size;
    extendedPart->part_start = partition->part_start+sizeof(EBR);
    extendedPart->part_status = Inactivo;
    writeEBR(extendedPart,path,partition->part_start);
}

void writeEBR(EBR *extendedPart,char path[],int point){
    FILE * myFile;
     myFile = fopen (path,"rb+");
     if (myFile==NULL)
     {
         cout<<"Error al abrir el archivo\n";
         return;
     }
     //escribir MBR en disco
     fseek(myFile, point, SEEK_SET);
     fwrite(extendedPart, sizeof(EBR), 1, myFile);
     //cerrando stream
     fclose (myFile);
}

Response newLogicPart(int size, Fit fit, char name[], MBR *disco, char path[]){
    EBR *firstEBR = getFirstEBR(disco,path);
    int newPosition = 0;
    if(firstEBR==NULL){
        return ERROR_NOT_EXIST_EXTENDED_PARTITION;
    }

    EBR *newEBR = (EBR*)malloc(sizeof(EBR));
    newEBR->part_fit = fit;
    strcpy(newEBR->part_name,name);
    newEBR->part_next = -1;
    newEBR->part_size = size;
    newEBR->part_status = Activo;
    //si es el unico que existe, reemplazar
    if(firstEBR->part_next == -1 && firstEBR->part_status == Inactivo){
       newEBR->part_start = firstEBR->part_start;
       writeEBR(newEBR,path,firstEBR->part_start-sizeof(EBR));
    }else{
        //agregar al final de la lista
        bool flag = true;
        while(flag){//existenExtendidas
            newPosition+=firstEBR->part_size;
            if(firstEBR->part_next!=-1){
               firstEBR = readEBR(firstEBR->part_next,path);
           }else{
               flag = false;
           }
        }

       firstEBR->part_next = newPosition;
       writeEBR(firstEBR,path,firstEBR->part_start-sizeof(EBR));
       newEBR->part_start = newPosition+sizeof(EBR);
       writeEBR(newEBR,path,newPosition);
    }



}
