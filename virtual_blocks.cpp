#include "virtual_blocks.h"

virtualBlock::virtualBlock(){
this->next = NULL;
}

virtualBlock::virtualBlock(int size, int start, statusBlock status){
    this->size = size;
    this->start = start;
    this->status = status;
}

virtualBlock_tracker::virtualBlock_tracker(){
    start = NULL;
}

void virtualBlock_tracker::addVirtualBlock(int size, int start,statusBlock status){
    if(this->start == NULL){
        this->start = new virtualBlock(size,start,status);
    }else{

    }
}

void virtualBlock_tracker::showTracker(){
    virtualBlock *node = this->start;
    while(node!=NULL){
        cout<<"start: "<<node->start<<endl;
        cout<<"size: "<<node->size<<endl;
    node = this->start->next;
    }
}

int virtualBlock_tracker::getNewPoint(Fit fit,int size){
    virtualBlock *node = this->start;
    //si todo el espacio está libre
    if(node->next == NULL){
        if(node->size<size){
            return -1;
        }
        return node->start;
    }

    while(node!=NULL){


    node = this->start->next;
    }
}

int getStartPoint(int size,MBR *disco,Fit fit){
    int i;
    int startPoint=-1;
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
       startPoint = sizeof(MBR);
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
        for (int i = 0;i < contadorParticiones; i++){
            for (int j = 0; j< contadorParticiones-1; j++){
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
                   startPoint = espaciosLibres[i]->start;
               }
            }
            if(startPoint == -1){
                return ERROR_INSUFICIENT_SPACE;
            }
            break;
        case BestFit:

            break;
        case WorstFit:
            if(size<=espaciosLibres[contadorEspacios-1]->size){
                startPoint = espaciosLibres[contadorEspacios-1]->start;
            }
            if(startPoint == -1){
                return ERROR_INSUFICIENT_SPACE;
            }
            break;
        }
    }
    return startPoint;
}
