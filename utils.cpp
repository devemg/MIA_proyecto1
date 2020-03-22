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

long getSize(int size,Unit unit){
    size_t final;
    switch(unit)
    {
       case Byte:
            final = size;
        break;
       case KB: //equivale a kilobytes
           final = size*1024;
        break;
       case MB: //equivale a MegaBytes
           final =  size * 1024;
           final = final *  1024;
        break;
  }
return final;
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
    case ERROR_DISK_NOT_MOUNTED:
        cout<<"El disco no está montado\n";
        break;
    case ERROR_NOT_SPACE_ADD:
        cout<<"No hay espacio para agregar a la partición\n";
        break;
    case ERROR_PARTITION_MOUNTED_DEL:
        cout<<"No se puede eliminar la partición al estar montada.\n";
        break;
    case ERROR_DIR_NOT_EXIST:
        cout<<"La carpeta no existe.\n";
        break;
    case ERROR_INODO_NOT_DIR:
        cout<<"El inodo no es de tipo carpeta\n";
        break;
    case ERROR_LEVEL_FULL:
        cout<<"No hay suficiente espacio para crear más directorios/archivos\n";
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
    fseek(myFile,position,SEEK_SET);
    while(ftell(myFile)<size && ftell(myFile)!=-1){
        fwrite("\0", sizeof(char), 1, myFile);
    }
    fclose(myFile);
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
            return -123456789;
        }
        catch (std::out_of_range const &e)
        {
            std::cout << "Integer overflow: std::out_of_range thrown" << '\n';
            return -123456789;
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
    case MB:return "MB";
    case KB:return "KB";
    case Byte:return "B";
    default:return "";
    }
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
        int sizeAll = strlen(s.c_str());
        s = s.substr(0,pos_);
        *poss = sizeAll-pos_;
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

Response getContadorDiscos(int *contadorDiscos,char *id){
    string str(id);
    if(strlen(id)<4){
        return ERROR_ID_MALFORMED;
    }
    char letra = str.at(2);
    //BUSCAR LETRA
    *contadorDiscos = 0;
    bool existeDisco= false;
    while(partsMounted[*contadorDiscos]!=NULL){
        if(partsMounted[*contadorDiscos]->letter == letra){
            existeDisco = true;
            break;
        }
        (*contadorDiscos)++;
    }
    if(!existeDisco){
        return ERROR_DISK_NOT_MOUNTED;
    }
    return SUCCESS;
}

void myPause(){
    system("read -r -p \"Presione cualquier tecla para continuar...\" key");
}
