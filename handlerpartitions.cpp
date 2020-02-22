#include "handlerpartitions.h"

//CREATE
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
        //newPartition(size,unit,full_path,tipoParticion,fit,name);
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
    if(final_size<=0){
        return ERROR_SIZE_MIN;
    }
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
     int startPoint;
     Response resp = getStartAddress(disco,fit,size,&startPoint);
     if(resp!=SUCCESS){
        return resp;
     }
     //BUSCAR PARTICION
     Partition *partition;
     bool hayEspacio = false;
 for(i=0;i<4;i++){
     partition  = &disco->particiones[i];
     if(partition->part_status==Inactivo){
         hayEspacio = true;
         break;
     }
 }
 if(hayEspacio){
         partition  = &disco->particiones[i];
         partition->part_fit = fit;
         strcpy(partition->part_name,name);
         partition->part_size = size;
         partition->part_start = startPoint;
         partition->part_type = Primaria;
         partition->part_status = Activo;
         replaceMBR(disco,path);
         //fillSpaceWithZeros(path,partition->part_start,size);
         return SUCCESS;
 }else{
            return ERROR_FULL_PARTITION_PRIMARY;
 }
}

Response getStartAddress(MBR *disco,Fit fit,int size,int *startPoint){
    int i;
    *startPoint = -1;
    int contadorParticiones = 0;
   //VIRTUALIZANDO ESPACIOS LIBRES
    virtualBlock *parts[4];

    bool empty = true;
    for(i = 0;i<4;i++){
        if(disco->particiones[i].part_status == Activo){
            parts[i] = new virtualBlock(disco->particiones[i].part_size,disco->particiones[i].part_start,OCUPADO);
            empty = false;
            contadorParticiones++;
        }
    }
    if(empty){
       *startPoint = sizeof(MBR);
    }else{
        //ordenar
        virtualBlock *temporal;
        for (int i = 0;i < contadorParticiones; i++){
            for (int j = 0; j< contadorParticiones-1; j++){
                if (parts[j]->start > parts[j+1]->start){
                temporal = parts[j];
                parts[j] = parts[j+1];
                parts[j+1] = temporal;
                }
            }
        }
       //obtener espacios libres
        int inicio=sizeof(MBR);
        int fin = sizeof(MB)-1;
        virtualBlock *espaciosLibres[5];
        int contadorEspacios = 0;
        for(i=0;i<contadorParticiones;i++){
           fin = parts[i]->start;
           if(fin - inicio > 0){
               espaciosLibres[contadorEspacios] = new virtualBlock(fin-inicio,inicio,LIBRE);
               contadorEspacios++;
           }
           inicio = parts[i]->start+parts[i]->size;
        }
        espaciosLibres[contadorEspacios] = new virtualBlock(disco->mbr_tamanio-inicio,inicio,LIBRE);
        contadorEspacios++;
        //ordenar espacios libres por tamaño
        for (int i = 0;i < contadorEspacios; i++){
            for (int j = 0; j< contadorEspacios-1; j++){
                if (espaciosLibres[j]->size > espaciosLibres[j+1]->size){
                temporal = espaciosLibres[j];
                espaciosLibres[j] = espaciosLibres[j+1];
                espaciosLibres[j+1] = temporal;
                }
            }
        }
        //BUSCAR SEGUN AJUSTE
        switch (fit) {
        case FirstFit:
            for(i=0;i<contadorEspacios;i++){
               if(size<=espaciosLibres[i]->size){
                   *startPoint = espaciosLibres[i]->start;
                   break;
               }
            }

            break;
        case WorstFit:
            if(size<=espaciosLibres[contadorEspacios-1]->size){
                *startPoint = espaciosLibres[contadorEspacios-1]->start;
            }
            break;
        case BestFit:
            int tams[contadorEspacios];
            for(i=0;i<contadorEspacios;i++){
                 tams[i] = espaciosLibres[i]->size-size;
            }

            int best =0;
            for(i=0;i<contadorEspacios;i++){
                if(tams[i]<tams[best]){
                    best = i;
                }
            }
            if(best == -1){
                return ERROR_INSUFICIENT_SPACE;
            }
            *startPoint = espaciosLibres[best]->start;
            break;
        }
    }
    if(*startPoint == -1){
        return ERROR_INSUFICIENT_SPACE;
    }
    return SUCCESS;
}

Response newExtendedPart(int size, Fit fit, char name[], MBR *disco, char path[]){
    int i;
    int startPoint;
    Response resp = getStartAddress(disco,fit,size,&startPoint);
    if(resp!=SUCCESS){
       return resp;
    }
    //BUSCAR PARTICION
    Partition *partition;
    bool noHayPartExt = true;
    bool hayEspacio = false;
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
        }
    }

        if(noHayPartExt){
            if(hayEspacio){
                partition  = &disco->particiones[i];
                    partition->part_fit = fit;
                    strcpy(partition->part_name,name);
                    partition->part_size = size;
                    partition->part_start=startPoint;
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
       if(size > firstEBR->part_size){
           return ERROR_INSUFICIENT_SPACE;
       }
       writeEBR(newEBR,path,firstEBR->part_start-sizeof(EBR));
    }else{

        //agregar al final de la lista
        bool flag = true;
        while(flag){//existenExtendidas
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

//DELETE
void deletePartition(char path[],char nameDisk[], char name[],TipoParticion tipoParticion,DeleteType dtype){
    char full_path[200];
    clearArray(full_path,sizeof(full_path));
    getFullPathDisk(path,nameDisk,full_path);
    //ESCRIBIR ARCHIVO
    Response response = rmPartition(full_path,name,tipoParticion,dtype);
    if(response == SUCCESS){
        char nameMirror[50];
        clearArray(nameMirror,50);
        strcat(nameMirror,nameDisk);
        strcat(nameMirror,"_mirror");
        clearArray(full_path,sizeof(full_path));
        getFullPathDisk(path,nameMirror,full_path);
        //ESCRIBIR RAID
        //Response response1 =
        rmPartition(full_path,name,tipoParticion,dtype);
        switch (tipoParticion) {
        case Primaria:
            cout<<"¡La partición primaria fue eliminada con éxito!\n";
            break;
        case Extendida:
            cout<<"¡La partición extendida fue eliminada con éxito!\n";
            break;
        case Logica:
            cout<<"¡La partición lógica fue eliminada con éxito!\n";
            break;
        }
        //if(response1 != SUCCESS)cout<<"Error al hacer cambio en el raid\n";
    }else{
        showMessageError(response);
    }
}

Response rmPartition(char path[], char name[], TipoParticion tipoParticion,DeleteType dtype){
    MBR *disco = openMBR(path);
    if(disco==NULL){
        return ERROR_UNHANDLED;
    }
    switch (tipoParticion) {
    case Primaria:
    case Extendida:
        return deletePrimaryPart(disco,name,dtype,path);
    case Logica:
        return deleteLogicPart(name,disco,path);
        break;
    }
}

Response deletePrimaryPart(MBR *disco,char name[],DeleteType dtype,char path[]){
    int i;
    for(i=0;i<4;i++){
        if(disco->particiones[i].part_status == Activo){
            if(strcmp(disco->particiones[i].part_name,name)==0){
                //delete partition
                disco->particiones[i].part_status = Inactivo;
                clearArray(disco->particiones[i].part_name,16);
                disco->particiones[i].part_size = 0;
                disco->particiones[i].part_start = 0;
                if(dtype == Full){
                  //fillSpaceWithZeros(path,0,0);
                }
                replaceMBR(disco,path);
                return SUCCESS;
            }
        }
    }
    return ERROR_PARTITION_NOT_EXIST;
}

Response deleteLogicPart(char name[], MBR *disco, char path[]){
    EBR *firstEBR = getFirstEBR(disco,path);
    Partition *extended = getExtendedPart(name,disco);
    if(extended==NULL){
        return ERROR_NOT_EXIST_EXTENDED_PARTITION;
    }
    int newPosition = 0;
    int fullSpace = 0;
    if(firstEBR==NULL){
        return ERROR_NOT_EXIST_EXTENDED_PARTITION;
    }

    EBR *backEBR = (EBR*)malloc(sizeof(EBR));

    if(firstEBR->part_next == -1 && firstEBR->part_status == Activo &&
            strcmp(firstEBR->part_name,name)){
       firstEBR->part_start = 0;
       clearArray(firstEBR->part_name,16);
       firstEBR->part_next = -1;
       firstEBR->part_size = 0;
       firstEBR->part_start = extended->part_start+sizeof(EBR);
       writeEBR(firstEBR,path,firstEBR->part_start-sizeof(EBR));
       return SUCCESS;
    }else{
        //agregar al final de la lista
        bool flag = true;
        while(flag){//existenExtendidas
            newPosition+=firstEBR->part_size;
            fullSpace += firstEBR->part_size;
            if(firstEBR->part_next!=-1){
                backEBR = firstEBR;
               firstEBR = readEBR(firstEBR->part_next,path);
           }else{
               flag = false;
           }
        }
        cout<<"HOLI";
/*       firstEBR->part_next = newPosition;
       writeEBR(firstEBR,path,firstEBR->part_start-sizeof(EBR));
       newEBR->part_start = newPosition+sizeof(EBR);
       writeEBR(newEBR,path,newPosition);
  */  }
}
