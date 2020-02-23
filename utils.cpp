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
       case MB: //equivale a MegaBytes
           return size * 1024 * 1024;
       default:
           return size * 1024 * 1024;
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
    case ERROR_EXISTS_EXTENDED_PARTITION:
        cout<<"Ya existe una partición extendida en el disco\n";
        break;
    case ERROR_NOT_EXIST_EXTENDED_PARTITION:
        cout<<"Error al operar la partición lógica. No existe una partición extendida en el disco\n";
        break;
    case ERROR_READING_EBR:
        cout<<"Ocurrió un error al crear la partición lógica :(\n";
        break;
    case ERROR_SIZE_MIN:
        cout<<"El tamaño deber ser mayor a cero\n";
        break;
    case ERROR_PARTITION_NOT_EXIST:
        cout<<"La partición no existe\n";
        break;
    default:
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

float getDecimal(float val){
    //float rounded_down = floorf(val * 100) / 100;
    return roundf(val * 100) / 100;
}

char* showOption(Options op){
    switch (op) {
    case Id:return "id";
    case Add:return "add";
    case Delete:return "delete";
    case Type:return "type";
    case Path:return "path";
    case Unitt:return "unit";
    case Fitt:return "fit";
    case Size:return "size";
    case Name:return "name";
    default:return "";
    }
}

const char* showFit(Fit op){
    switch (op) {
    case FirstFit:return "FF";
    case WorstFit:return "WF";
    case BestFit:return "BF";
    default:return "";
    }
}

const char* showUnit(Unit op){
    switch (op) {
    case MB:return "KB";
    case KB:return "MB";
    case Byte:return "B";
    default:return "";
    }
}

string getNamePath(char *path){
    string s(path);
    string delimiter = "/";

    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        //std::cout << token << std::endl;
        s.erase(0, pos + delimiter.length());
    }
    int pos_ = -1;
    pos_ = s.find(".");
    if(pos_>0){
        s = s.substr(0,pos_);
    }
    return s;
}

string getPathWithoutName(char *path,int sizeName){
    string s(path);
    int size=strlen(path);
    if(size-sizeName-1<size && size-sizeName-1 >0){
        return s.substr(0,size-sizeName-1);
    }
    return "";
}

void letsExecCommands(Command *commands){
    Command *first = commands;
    char *name;
    char *path;
    char *id;
    Fit fit;
    Unit unit;
    int size = -1;
    string ss;
    string hh;
    char *chh;
    Option *it;
    while(first!=NULL){
        switch (first->cmd) {
        case mkdisk:
            fit = FirstFit;
            unit = MB;
            it = first->opts;
            while(it!=NULL){
                switch (it->option) {
                case Size:
                    size = it->num;
                    break;
                case Fitt:
                    fit = it->fit;
                    break;
                case Unitt:
                    unit = it->unit;
                    break;
                case Path:
                    path = it->text;
                    break;
                }
             it = it->next;
            }
            //cout<<"PATH COMPLETE: "<<path<<endl;
            //cout<<"SIZE: "<<size <<endl;
            //cout<<"FIT: "<<showFit(fit)<<endl;
            //cout<<"UNIT: "<<showUnit(unit)<<endl;
            ss = getNamePath(path);
            chh = &ss[0];
            //cout<<"NAME: "<<chh<<endl;
            hh = getPathWithoutName(path,strlen(chh));
            //cout<<hh<<endl;
            newDisk(size,fit,unit,&hh[0],chh);
            break;
        }
        first = first->next;
    }
}
