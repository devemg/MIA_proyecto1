#include "utils.h"

Command* ListCommand(Command *cmd,bool flag){
    static Command *commandList;
    if(flag){
       commandList  = cmd;
    }
    return commandList;
}

void writeCommand(string command,bool isScript){
    FILE * pFile;
    if(!isScript){
        pFile = fopen ("command.txt", "w+");
    }else{
        pFile = fopen ("command_script.txt", "w+");
    }
      fwrite (command.c_str(), sizeof(char), strlen(command.c_str()), pFile);
      fclose (pFile);
}

void readExecCommand(bool isScript){
            FILE* input;
            if(!isScript){
                input = fopen ("command.txt", "r");
            }else{
                input = fopen ("command_script.txt", "r");
            }
            if(input == NULL){
                cout<<"Error al leer script\n";
            }
            yyrestart(input);//SE PASA LA CADENA DE ENTRADA A FLEX
            int res = yyparse();//SE INICIA LA COMPILACION
            if(res == 0){
                if(ListCommand(NULL,false)!=NULL){
                    letsExecCommands(ListCommand(NULL,false));
                    //limpiar comando
                    ListCommand(NULL,true);
                }
            }
            fclose(input);
}

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
    case exec:
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
        existId = false;
        while(it!=NULL){
            switch (it->option) {
            case Id:
                existId = true;
                break;
            default:
                std::cout<<"Opción \""<<showOption(it->option)<<"\" no válida para el comando.\n";
                break;
            }
         it = it->next;
        }
        if(!existId){
            std::cout<<"Debe indicar un id de partición.\n";
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
                if(strcmp(it->text,"mbr")!=0 && strcmp(it->text,"disk")!=0 && strcmp(it->text,"mount")!=0){
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
        {//opciones obligatorias
        //path name id
        it = opt;
        existPath = false;
        existName = false;
        existId = false;
        bool isDelete = false;
        bool isAdd = false;
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
            case Type:
            case Fitt:
                break;
            case Add:
                isAdd = true;
                break;
            case Delete:
                isDelete = true;
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
        if(!isDelete && !isAdd)if(!existSize){
            std::cout<<"Debe indicar un tamaño.\n";
            return false;
        }
        }break;
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

void fillOptions(Option *it,int *size, int *add,Fit *fit,Unit *unit,TipoParticion *tipo,DeleteType *deltype,
                 char **path,char **id,char **name,bool *isAdd,bool *isDelete){
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
            *isDelete = true;
            *deltype = it->delType;
            break;
        case Add:
            *isAdd = true;
            *add = it->num;
            break;
        }
     it = it->next;
    }
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
    bool isAdd;
    bool isDelete;
    DeleteType delType;
    TipoParticion tipoPart;
    Option *it;
    while(first!=NULL){
        switch (first->cmd) {
        case mkdisk:
        {
            fit = FirstFit;
            unit = MB;
            it = first->opts;
            fillOptions(it,&size,&add,&fit,&unit,&tipoPart,&delType,&path,&id,&name,&isAdd,&isDelete);
            /*
            cout<<"PATH COMPLETE: "<<path<<endl;
            cout<<"SIZE: "<<size <<endl;
            cout<<"FIT: "<<showFit(fit)<<endl;
            cout<<"UNIT: "<<showUnit(unit)<<endl;
            */
            int ext = 0;
            ss = getNamePath(path,&ext);
            chh = &ss[0];
            hh = getPathWithoutName(path,strlen(chh)+ext);
            //cout<<"NAME: "<<chh<<endl;
            //cout<<"PATH: "<<hh<<endl;
            newDisk(size,fit,unit,&hh[0],chh);
        }
            break;
        case rmdisk:
            it = first->opts;
            fillOptions(it,&size,&add,&fit,&unit,&tipoPart,&delType,&path,&id,&name,&isAdd,&isDelete);
            //cout<<"PATH COMPLETE: "<<path<<endl;
            deleteDisk(path);
            break;
        case mount:
        {  it = first->opts;
            fillOptions(it,&size,&add,&fit,&unit,&tipoPart,&delType,&path,&id,&name,&isAdd,&isDelete);
            //cout<<"PATH COMPLETE: "<<path<<endl;
            //cout<<"NAME: "<<name<<endl;
            Response res = mountPart(path,name);
            if(res!=SUCCESS){
                showMessageError(res);
                return;
            }else{
                showMounts();
                cout<<"¡Partición montada con éxito!\n";
            }
        }
            break;
        case unmount:
        {    it = first->opts;
            fillOptions(it,&size,&add,&fit,&unit,&tipoPart,&delType,&path,&id,&name,&isAdd,&isDelete);
            //cout<<"ID: "<<id<<endl;        
            Response res = unmountPart(id);
            if(res!=SUCCESS){
                showMessageError(res);
                return;
            }else{
                showMounts();
                cout<<"¡Partición desmontada con éxito!\n";
            }
        }
            break;
        case rep:
         {   it = first->opts;

            fillOptions(it,&size,&add,&fit,&unit,&tipoPart,&delType,&path,&id,&name,&isAdd,&isDelete);

            /*cout<<"ID: "<<id<<endl;
            cout<<"PATH COMPLETE: "<<path<<endl;
            cout<<"NAME: "<<name<<endl;
            */
            //BUSCAR DISCO
            if(strcmp(name,"mount")==0){
                showMounts();
                return;
            }
            int contadorDiscos;
            Response res = getContadorDiscos(&contadorDiscos,id);
            if(res!=SUCCESS){
                showMessageError(res);
                return;
            }

        MountedDisk *disk = partsMounted[contadorDiscos];
        int ext = 0;
        ss = getNamePath(path,&ext);
        chh = &ss[0];
        hh = getPathWithoutName(path,strlen(chh)+ext);

        //cout<<"PATH: "<<hh<<endl;
        //CREAR DIRECTORIO SI NO EXISTE
        mkdir(hh.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

            if(strcmp(name,"mbr")==0){
                reportMBR(disk->path,path);
            }else{
                reportDisk(disk->path,path);
            }
       }
        break;
        case exec:
        {
            it = first->opts;
            fillOptions(it,&size,&add,&fit,&unit,&tipoPart,&delType,&path,&id,&name,&isAdd,&isDelete);
            //cout<<"PATH COMPLETE: "<<path<<endl;
            //abrir archivo
            cout<<"Ejecutando script...\n\n";
            FILE *myFile = fopen(path,"r");
            if(myFile==NULL){
                cout<<"Error al abrir el script.\n";
                break;
            }
            char linea[1024];
            while(fgets(linea, 1024, (FILE*) myFile)) {
                    cout<<"---->"<<linea;
                    writeCommand(linea,true);
                    readExecCommand(true);
                }
           fclose(myFile);
        }
            break;
        case fdisk:
        {   it = first->opts;
            size = 0;
            unit = KB;
            tipoPart = Primaria;
            fit = WorstFit;
            delType = Fast;
            isDelete = false;
            isAdd = false;
            add = 0;
            fillOptions(it,&size,&add,&fit,&unit,&tipoPart,&delType,&path,&id,&name,&isAdd,&isDelete);
            /*
            cout<<"PATH COMPLETE: "<<path<<endl;
            cout<<"SIZE: "<<size<<endl;
            cout<<"UNIT: "<<showUnit(unit)<<endl;
            cout<<"TIPO PART: "<<tipoPart<<endl;
            cout<<"FIT: "<<showFit(fit)<<endl;
            cout<<"DELTYPE: "<<delType<<endl;
            cout<<"NAME: "<<name<<endl;
            cout<<"DELTYPE: "<<delType<<endl;
            */
            int ext = 0;
            ss = getNamePath(path,&ext);
            chh = &ss[0];
            hh = getPathWithoutName(path,strlen(chh)+ext);
            //cout<<"NAME: "<<chh<<endl;
            //cout<<"PATH: "<<hh<<endl;
            if(isDelete){
                //eliminar particion
                Response res = deletePartition(&hh[0],chh,name,tipoPart,delType);
                if(res==SUCCESS){
                    cout<<"¡Partición eliminada con éxito!\n";
                }else{
                    showMessageError(res);
                }
            }else if(isAdd){
                //modificar particion
                Response res = updatepartition(&hh[0],chh,name,add,unit);
                if(res == SUCCESS){
                    cout<<"¡La partición fue modificada cón éxito!\n";
                }else{
                    showMessageError(res);
                }
            }else{
            Response res =  createPartition(size,unit,&hh[0],chh,tipoPart,fit,name);
            if(res == SUCCESS){
                switch (tipoPart) {
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
            }else{
                showMessageError(res);
            }
            }
        }break;
        }
        first = first->next;
    }
}

void myPause(){
    system("read -r -p \"Presione cualquier tecla para continuar...\" key");
}
