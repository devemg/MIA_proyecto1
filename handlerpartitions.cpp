#include "handlerpartitions.h"

void createPartition(int size, Unit unit, char path[],char nameDisk[],TipoParticion tipoParticion, Fit fit, char name[]){
    newPartition(size,unit,"/home/emely/Escritorio/testData/disk1.disk",tipoParticion,fit,name);
    newPartition(size,unit,"/home/emely/Escritorio/testData/disk1_mirror.disk",tipoParticion,fit,name);
}

void newPartition(int size, Unit unit, char path[], TipoParticion tipoParticion, Fit fit, char name[]){
    MBR *disco = openMBR(path);
    if(disco==NULL){
        return;
    }
    int final_size = getSize(size,unit);
    switch (tipoParticion) {
    case Primaria:
        newPrimaryPart(final_size,fit,name,disco);
        replaceMBR(disco,path);
        break;
    case Extendida:
        break;
    case Logica:
        break;
    }
}

void newPrimaryPart(int size,Fit fit,char name[],MBR *disco){
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
            cout<<"Error al crear la partición. No hay espacio suficiente\n";
            return;
        }
            partition  = &disco->particiones[i];
                partition->part_fit = fit;
                strcpy(partition->part_name,name);
                partition->part_size = size;
                partition->part_type = Primaria;
                partition->part_status = Activo;
                cout<<"¡La partición primaria fue creada con éxito!\n";
    }else{
        cout<<"Error al crear la partición. Ya existen cuatro particiones creadas\n";
    }
}
