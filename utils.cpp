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
    case ERROR_PARTITION_EXIST:
        cout<<"La partición ya existe\n";
        break;
    case ERROR_PARTITION_MOUNTED:
        cout<<"La partición ya está montada\n";
        break;
    case ERROR_ID_MALFORMED:
        cout<<"Id incorrecto\n";
        break;
    case ERROR_DISK_NOT_EXIST:
        cout<<"El disco no existe\n";
        break;
    case ERROR_PARTITION_NOT_MOUNTED:
        cout<<"La partición no fue montada\n";
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

int getInt(string s){
    try
        {
            int i = std::stoi(s);
            return i;
        }
        catch (std::invalid_argument const &e)
        {
            std::cout << "Bad input: std::invalid_argument thrown" << '\n';
            return -12345678910;
        }
        catch (std::out_of_range const &e)
        {
            std::cout << "Integer overflow: std::out_of_range thrown" << '\n';
            return -12345678910;
        }
}

float getDecimal(float val){
    //float rounded_down = floorf(val * 100) / 100;
    return roundf(val * 100) / 100;
}

const char* showOption(Options op){
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

bool validateOptionCommand(CommandEnum cmd,Option *opt){
    bool existName;
    bool existPath;
    bool existId;
    bool existSize;
    Option *it;

    switch (cmd) {
    case mkdisk:
        //opciones obligatorias
        //size path
        existPath = false;
        existSize = false;
        it = opt;
        while(it!=NULL){
            switch (it->option) {
            case Size:
                if(it->num > 0){
                    existSize=true;
                }else{
                    std::cout<<"El tamaño debe ser mayor o igual a cero. \n";
                    return false;
                }
                break;
            case Fitt:
            case Unitt:
                break;
            case Path:
                existPath = true;
                break;
            default:
                std::cout<<"Opción \""<<showOption(it->option)<<"\" no válida para el comando.\n";
                break;
            }
         it = it->next;
        }
        if(!existSize){
            std::cout<<"Debe indicar un tamaño.\n";
            return false;
        }
        if(!existPath){
            std::cout<<"Debe indicar un archivo.\n";
            return false;
        }
        break;
    case rmdisk:
        //opciones obligatorias
        //path
        existPath = false;
         it = opt;
        while(it!=NULL){
            switch (it->option) {
            case Path:
                existPath = true;
                break;
            default:
                std::cout<<"Opción \""<<showOption(it->option)<<"\" no válida para el comando.\n";
                break;
            }
         it = it->next;
        }
        if(!existPath){
            std::cout<<"Debe indicar un archivo.\n";
            return false;
        }
        break;
    case mount:
        //opciones obligatorias
        //path name
        existPath = false;
        existName = false;
        it = opt;
        while(it!=NULL){
            switch (it->option) {
            case Path:
                existPath = true;
                break;
            case Name:
                existName = true;
                break;
            default:
                std::cout<<"Opción \""<<showOption(it->option)<<"\" no válida para el comando.\n";
                break;
            }
         it = it->next;
        }
        if(!existPath){
            std::cout<<"Debe indicar un archivo.\n";
            return false;
        }
        if(!existName){
            std::cout<<"Debe indicar un nombre.\n";
            return false;
        }
        break;
    case unmount:
        //opciones obligatorias
        //name
        it = opt;
        existName = false;
        while(it!=NULL){
            switch (it->option) {
            case Name:
                existName = true;
                break;
            default:
                std::cout<<"Opción \""<<showOption(it->option)<<"\" no válida para el comando.\n";
                break;
            }
         it = it->next;
        }
        if(!existName){
            std::cout<<"Debe indicar un nombre.\n";
            return false;
        }
        break;
   case rep:
        //opciones obligatorias
        //path name id
        it = opt;
        existPath = false;
        existName = false;
        existId = false;
        int len;
        int index;
        while(it!=NULL){
            switch (it->option) {
            case Path:
                existPath = true;
                break;
            case Name:
                existName = true;
                len = strlen(it->text);
                for (index = 0; index < len; ++index)
                   it->text[index] = tolower(it->text[index]);
                if(strcmp(it->text,"mbr")!=0 && strcmp(it->text,"disk")!=0){
                    std::cout<<"Debe indicar el tipo de reporte: \"mbr\" o \"disk\".\n";
                    return false;
                }

                break;
            case Id:
                existId = true;
                break;
            default:
                std::cout<<"Opción \""<<showOption(it->option)<<"\" no válida para el comando.\n";
                break;
            }
         it = it->next;
        }
        if(!existPath){
            std::cout<<"Debe indicar un archivo.\n";
            return false;
        }
        if(!existName){
            std::cout<<"Debe indicar un nombre.\n";
            return false;
        }
        if(!existId){
            std::cout<<"Debe indicar un id.\n";
            return false;
        }
        break;
     case fdisk:
        //opciones obligatorias
        //path name id
        it = opt;
        existPath = false;
        existName = false;
        existId = false;
        while(it!=NULL){
            switch (it->option) {
            case Size:
                if(it->num > 0){
                    existSize = true;
                }else{
                    std::cout<<"El tamaño debe ser mayor o igual a cero. \n";
                    return false;
                }
                break;
            case Path:
                existPath=true;
                break;
            case Name:
                existName = true;
                break;
            case Id:
                existId = true;
                break;
            case Unitt:
            case Add:
            case Type:
            case Fitt:
            case Delete:
                break;
            default:
                std::cout<<"Opción \""<<showOption(it->option)<<"\" no válida para el comando.\n";
                break;
            }
         it = it->next;
        }
        if(!existPath){
            std::cout<<"Debe indicar un archivo.\n";
            return false;
        }
        if(!existName){
            std::cout<<"Debe indicar un nombre.\n";
            return false;
        }
        if(it->option != Delete)if(!existId){
            std::cout<<"Debe indicar un id.\n";
            return false;
        }
        if(it->option != Delete)if(!existSize){
            std::cout<<"Debe indicar un tamaño.\n";
            return false;
        }
        break;
    }
    return true;
}

string getNamePath(char *path,int *poss){
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
        *poss = pos_;
    }
    return s;
}

string getPathWithoutName(char *path,int sizeName){
    string s(path);
    int size=strlen(path);
    if(size-sizeName-1<size && size-sizeName-1 >0){
        return s.substr(0,size-sizeName-1);
    }
    return s;
}

void fillOptions(Option *it,int *size, int *add,Fit *fit,Unit *unit,TipoParticion *tipo,DeleteType *deltype,
                 char **path,char **id,char **name){
    while(it!=NULL){
        switch (it->option) {
        case Size:
            *size = it->num;
            break;
        case Fitt:
            *fit = it->fit;
            break;
        case Unitt:
            *unit = it->unit;
            break;
        case Path:
            *path = it->text;
            break;
        case Id:
            *id = it->text;
            break;
        case Name:
            *name = it->text;
            break;
        case Type:
            *tipo = it->type;
            break;
        case Delete:
            *deltype = it->delType;
            break;
        case Add:
            *add = it->num;
            break;
        }
     it = it->next;
    }
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
    int add;
    DeleteType delType;
    TipoParticion tipoPart;
    Option *it;
    while(first!=NULL){
        switch (first->cmd) {
        case mkdisk:
            fit = FirstFit;
            unit = MB;
            it = first->opts;
            fillOptions(it,&size,&add,&fit,&unit,&tipoPart,&delType,&path,&id,&name);

            cout<<"PATH COMPLETE: "<<path<<endl;
            cout<<"SIZE: "<<size <<endl;
            cout<<"FIT: "<<showFit(fit)<<endl;
            cout<<"UNIT: "<<showUnit(unit)<<endl;

            int ext = 0;
            ss = getNamePath(path,&ext);
            chh = &ss[0];
            cout<<"NAME: "<<chh<<endl;
            hh = getPathWithoutName(path,strlen(chh)+ext);
            cout<<"PATH: "<<hh<<endl;
            //newDisk(size,fit,unit,&hh[0],chh);
            break;
        }
        first = first->next;
    }
}
