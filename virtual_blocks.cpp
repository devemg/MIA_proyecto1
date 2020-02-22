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
