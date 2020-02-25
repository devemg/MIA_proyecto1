#include "handlerpartitions.h"
#include <vars.h>

Response createPartition(int size, Unit unit, char path[],char nameDisk[],TipoParticion tipoParticion, Fit fit, char name[]){
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
        //if(response1 != SUCCESS)cout<<"Error al hacer cambio en el raid\n";
    }
    return response;
}

Response updatepartition(char path[],char nameDisk[],char namePart[],int sizeToMod,Unit unit){
    char full_path[200];
    clearArray(full_path,sizeof(full_path));
    getFullPathDisk(path,nameDisk,full_path);
    //ESCRIBIR ARCHIVO
    Response response = modPartition(sizeToMod,unit,full_path,namePart);
    if(response == SUCCESS){
        char nameMirror[50];
        clearArray(nameMirror,50);
        strcat(nameMirror,nameDisk);
        strcat(nameMirror,"_mirror");
        clearArray(full_path,sizeof(full_path));
        getFullPathDisk(path,nameMirror,full_path);
        //ESCRIBIR RAID
        //Response response1 =
        modPartition(sizeToMod,unit,full_path,namePart);
        //if(response1 != SUCCESS)cout<<"Error al hacer cambio en el raid\n";
    }
    return response;

}

Response newPartition(int size, Unit unit, char path[], TipoParticion tipoParticion, Fit fit, char name[]){
    MBR *disco = openMBR(path);
    if(disco==NULL){
        return ERROR_UNHANDLED;
    }
    long final_size = getSize(size,unit);
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
    return SUCCESS;
}

Response modPartition(int size, Unit unit, char path[], char name[]){
    MBR *disco = openMBR(path);
    if(disco==NULL){
        return ERROR_UNHANDLED;
    }
    long final_size = getSize(size,unit);

    virtualBlock *parts[4];
    int contadorParticiones = 0;
    int i;
    for(i = 0;i<4;i++){
        if(disco->particiones[i].part_status == Activo){
            parts[contadorParticiones] = new virtualBlock(disco->particiones[i].part_size,disco->particiones[i].part_start,OCUPADO);
            contadorParticiones++;
        }
    }
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
    //buscar SIGUIENTE particion o espacio antes del fin del disco
    Partition *part = NULL;
    int indicePart = 0;
    for(i = 0;i<4;i++){
        if(disco->particiones[i].part_status == Activo){
            if(strcmp(disco->particiones[i].part_name,name)==0){
                part = &disco->particiones[i];
                break;
            }
            indicePart++;
        }
    }


    if(part==NULL){
        //BUSCAR PARTICION LOGICA
        //SI NO EXISTE, RETORNAR ERROR
        return ERROR_PARTITION_NOT_EXIST;
    }else{
        //MODIFICAR TAMAÑO
        if(part->part_size+final_size<=0){
            return ERROR_SIZE_MIN;
        }
        if(indicePart == contadorParticiones-1){
            int newt = part->part_start+part->part_size+final_size;
            if(disco->mbr_tamanio < newt){
                return ERROR_NOT_SPACE_ADD;
            }
        }else{
            int newt = part->part_start+part->part_size+final_size;
            if(parts[indicePart+1]->start < newt){
                return ERROR_NOT_SPACE_ADD;
            }
        }
        part->part_size = part->part_size+final_size;
        replaceMBR(disco,path);
        //ESCRIBIR MBR
    }
    return SUCCESS;
}

Response newPrimaryPart(long size,Fit fit,char name[],MBR *disco,char path[]){
     int i;
     int startPoint;
     Response resp = getStartAddress(disco,(Fit)disco->disk_fit,size,&startPoint);
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
     }else{
         if(strcmp(partition->part_name,name)==0){
             return ERROR_PARTITION_EXIST;
         }
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
         fillSpaceWithZeros(path,partition->part_start,size);
         return SUCCESS;
 }else{
            return ERROR_FULL_PARTITION_PRIMARY;
 }
}

Response getStartAddress(MBR *disco,Fit fit,long size,int *startPoint){
    int i;
    *startPoint = -1;
    int contadorParticiones = 0;
   //VIRTUALIZANDO particiones
    virtualBlock *parts[4];

    bool empty = true;
    for(i = 0;i<4;i++){
        if(disco->particiones[i].part_status == Activo){
            parts[contadorParticiones] = new virtualBlock(disco->particiones[i].part_size,disco->particiones[i].part_start,OCUPADO);
            empty = false;
            contadorParticiones++;
        }
    }
    if(empty){
        if(disco->mbr_tamanio>=size){
            *startPoint = sizeof(MBR);
        }
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
        espaciosLibres[contadorEspacios] = new virtualBlock(disco->mbr_tamanio-inicio+sizeof(MBR),inicio,LIBRE);
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

Response newExtendedPart(long size, Fit fit, char name[], MBR *disco, char path[]){
    int i;
    int startPoint;
    Response resp = getStartAddress(disco,(Fit)disco->disk_fit,size,&startPoint);
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
        }else{
            if(strcmp(partition->part_name,name)==0){
                return ERROR_PARTITION_EXIST;
            }
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
                    fillSpaceWithZeros(path,partition->part_start+sizeof(EBR),size);
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

Response newLogicPart(long size, Fit fit, char name[], MBR *disco, char path[]){
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
       newPosition = firstEBR->part_start+firstEBR->part_size;
       firstEBR->part_next = newPosition;
       writeEBR(firstEBR,path,firstEBR->part_start-sizeof(EBR));
       newEBR->part_start = newPosition+sizeof(EBR);
       writeEBR(newEBR,path,newPosition);
    }


return SUCCESS;
}

Response deletePartition(char path[],char nameDisk[], char name[],TipoParticion tipoParticion,DeleteType dtype){
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
        //if(response1 != SUCCESS)cout<<"Error al hacer cambio en el raid\n";
    }
    return response;
}

Response rmPartition(char path[], char name[], TipoParticion tipoParticion,DeleteType dtype){
    MBR *disco = openMBR(path);
    if(disco==NULL){
        return ERROR_UNHANDLED;
    }
    /*if(!canDeletePart(path,name)){
        return ERROR_PARTITION_MOUNTED_DEL;
    }*/
    switch (tipoParticion) {
    case Primaria:
    case Extendida:
        return deletePrimaryPart(disco,name,dtype,path);
    case Logica:
        return deleteLogicPart(name,disco,path);
        break;
    }
    return SUCCESS;
}

Response deletePrimaryPart(MBR *disco,char name[],DeleteType dtype,char path[]){
    int i;
    for(i=0;i<4;i++){
        if(disco->particiones[i].part_status == Activo){
            if(strcmp(disco->particiones[i].part_name,name)==0){
                //delete partition
                disco->particiones[i].part_status = Inactivo;
                clearArray(disco->particiones[i].part_name,16);
                if(dtype == Full){
                  fillSpaceWithZeros(path,disco->particiones[i].part_start,disco->particiones[0].part_size);
                }
                disco->particiones[i].part_size = 0;
                disco->particiones[i].part_start = 0;
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
    return SUCCESS;
}

Response mountPart(char path[], char name[]){

    int contador = 0;
    bool exist= false;
    //MONTAR DISCO
    while(partsMounted[contador]!=NULL){
        if(strcmp(partsMounted[contador]->path,path)==0){
            exist = true;
            break;
        }
        contador++;
    }
    if(!exist){
        partsMounted[contador] = new MountedDisk();
        strcpy(partsMounted[contador]->path,path);
        partsMounted[contador]->letter = 97+contador;
    }
    //MONTAR PARTICION
    bool existPart = false;
    MountedDisk *mdisk = partsMounted[contador];
    int contador2=0;
    //VALIDAR QUE YA ESTÉ MONTADA LA PARTICION
    while(mdisk->parts[contador2]!=NULL){
        if(strcmp(mdisk->parts[contador2]->name,name)==0){
            return ERROR_PARTITION_MOUNTED;
        }
        contador2++;
    }

    MBR *disco = openMBR(mdisk->path);
    if(disco==NULL){
        return ERROR_UNHANDLED;
    }
    //BUSCAR PARTICION PRIMARIA/EXTENDIDA
    int i;
    for(i=0;i<4;i++){
        if(strcmp(disco->particiones[i].part_name,name)==0){
            existPart = true;
            break;
        }
    }

    if(existPart){
         mdisk->parts[contador2] = new MountedPart();
         strcpy(mdisk->parts[contador2]->id,getPartId(mdisk->letter,contador2));
         strcpy(mdisk->parts[contador2]->name, name);
    }else{
        return ERROR_PARTITION_NOT_EXIST;
    }
    return SUCCESS;
}

void showMounts(){
    int contador = 0;
    int contador2=0;
    while(partsMounted[contador]!=NULL){
        cout<<"--------------------------------------------------\n";
        cout<<"DISCO \""<<partsMounted[contador]->letter<<"\" \n";
        cout<<partsMounted[contador]->path<<endl;
        cout<<"Particiones: \n";
            contador2 = 0;
            while (partsMounted[contador]->parts[contador2]!=NULL) {
                cout<<"Nombre: "<<partsMounted[contador]->parts[contador2]->name<<endl;
                cout<<"Id: "<<partsMounted[contador]->parts[contador2]->id<<endl;
                contador2++;
            }
        contador++;
    }
    cout<<"--------------------------------------------------\n";
}

char* getPartId(char letra, int numero){
    string str("vd");
    str+=letra;
    str+=to_string(numero);
    return &str[0];
}

Response unmountPart(char id[]){
    string str(id);
    if(strlen(id)<4){
        return ERROR_ID_MALFORMED;
    }
    char letra = str.at(2);
    //BUSCAR LETRA
    int contadorDiscos = 0;
    bool existeDisco= false;
    while(partsMounted[contadorDiscos]!=NULL){
        if(partsMounted[contadorDiscos]->letter == letra){
            existeDisco = true;
            break;
        }
        contadorDiscos++;
    }
    if(!existeDisco){
        return ERROR_DISK_NOT_EXIST;
    }
    //BUSCAR NUMERO
    int contadorPart = 0;
    int sizeParts= 0;
    bool existePart = false;

    while((partsMounted[contadorDiscos])->parts[sizeParts]!=NULL){
        sizeParts++;
    }

    while((partsMounted[contadorDiscos])->parts[contadorPart]!=NULL){
        if(strcmp((partsMounted[contadorDiscos])->parts[contadorPart]->id,id)==0){
            existePart = true;
            break;
        }
        contadorPart++;
    }
    if(existePart){
        delete (partsMounted[contadorDiscos])->parts[contadorPart];
        (partsMounted[contadorDiscos])->parts[contadorPart] = NULL;
        sizeParts--;
        if(sizeParts == 0){
            //desmontar disco
            while((partsMounted[contadorDiscos])!=NULL){
                delete partsMounted[contadorDiscos];
                (partsMounted[contadorDiscos]) = (partsMounted[contadorDiscos+1]);
            }
            (partsMounted[contadorDiscos]) = NULL;
        }
    }else{
        return ERROR_PARTITION_NOT_MOUNTED;
    }
    return SUCCESS;
}
