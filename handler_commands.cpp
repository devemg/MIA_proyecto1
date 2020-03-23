#include "handler_commands.h"

cmd_mkdisk::cmd_mkdisk(int size, char path[]){
    this->size = size;
    this->path = path;
    this->fit = FirstFit;
    this->unit = MB;
}

cmd_rmdisk::cmd_rmdisk(char path[]){
    this->path = path;
}

cmd_fdisk::cmd_fdisk(char name[], char path[], int size){
    this->name = name;
    this->path = path;
    this->size = size;
}

cmd_addPart::cmd_addPart(char name[], char path[], int size){
    this->size = size;
    this->name = name;
    this->path = path;
    this->unit = KB;
}

cmd_rmPart::cmd_rmPart(char name[], char path[],TypeFormat type){
    this->name = name;
    this->path = path;
    this->typeDel = type;
}

cmd_mount::cmd_mount(char path[], char name[]){
    this->path = path;
    this->name = name;
}

cmd_unmount::cmd_unmount(char id[]){
    this->id = id;
}

cmd_fs::cmd_fs(char id[]){
    type = Full;
    this->id = id;
    fs = ext2;
}

cmd_login::cmd_login(char usr[], char pwd[], char id[]){
    this->usr = usr;
    this->pwd = pwd;
    this->id = id;
}

cmd_grp::cmd_grp(char name[], bool isForCreate){
    this->name = name;
    this->isForCreate = isForCreate;
}

cmd_mkusr::cmd_mkusr(char usr[], char pwd[], char grp[]){
    this->usr = usr;
    this->pwd = pwd;
    this->grp = grp;
}

cmd_rmusr::cmd_rmusr(char usr[]){
    this->usr = usr;
}

cmd_chmod::cmd_chmod(char path[], int ugo){
    this->path = path;
    this->ugo = ugo;
    this->isRecursive = false;
}

cmd_mkfile::cmd_mkfile(char path[]){
    this->path = path;
    this->isRecursive = false;
    this->size = 0;
    this->cont = NULL;
}

cmd_file::cmd_file(char path[], bool isForCat){
    this->path = path;
    this->isForCat = isForCat;
}

cmd_edit::cmd_edit(char path[], char cont[]){
    this->path = path;
    this->cont = cont;
}

cmd_ren::cmd_ren(char path[], char newName[]){
    this->path = path;
    this->newName = newName;
}

cmd_mkdir::cmd_mkdir(char path[]){
    this->isRecursive = false;
    this->path = path;
}

cmd_cp::cmd_cp(char path[], char newPath[]){
    this->path = path;
    this->newPath = newPath;
}

cmd_find::cmd_find(char path[], char name[]){
    this->path = path;
    this->name = name;
}

cmd_chown::cmd_chown(char path[], char usr[]){
    this->path = path;
    this->isRecursive = false;
    this->usr = usr;
}

cmd_chgrp::cmd_chgrp(char usr[], char grp[]){
    this->usr = usr;
    this->grp = grp;
}

cmd_loss::cmd_loss(char id[]){
    this->id = id;
}

cmd_rep::cmd_rep(char path[], TypeReport type, char id[]){
    this->path_report = path;
    this->type = type;
    this->id = id;
    this->path = NULL;
}

void cmd_rep::Exec(){
    cout<<"ejecutando rep..\n";
    cout<<this->id<<endl;
    cout<<this->path<<endl;
    cout<<this->path_report<<endl;
    cout<<this->type<<endl;
}

Cmd* getFormedCommand(CommandEnum command,Option *op, Cmd *cmd){
    Option *it = op;

    char *name = NULL;
    char *path = NULL;
    char *id = NULL;
    int size = -1;
    Fit fit = FIT_ERROR;
    Unit unit = UNIT_ERROR;
    TypeFormat typeFormat = TF_ERROR;
    TypePartition typePartition = TP_ERROR;
    bool existSize = false;
    bool existAdd = false;
    while(it!=NULL){
        switch (it->option) {
        case Name:
            name = it->text;
            break;
        case Path:
            path = it->text;
            break;
        case Id:
            id = it->text;
            break;
        case Size:
            size = it->num;
            existSize = true;
            break;
        case Fitt:
            fit = it->fit;
            break;
        case Unitt:
            unit = it->unit;
            break;
        case Type:
            typePartition = it->type;
            break;
        case Delete:
            typeFormat = it->delType;
            break;
        case Add:
            size = it->num;
            existAdd = true;
            break;
        }
          it = it->next;
      }
      switch (command) {
      case mkdisk:
      {
          if(!existSize){
              std::cout<<"Debe indicar un tamaño(-size).\n";
              return NULL;
          }
          if(path!=NULL){
              std::cout<<"Debe indicar una ruta(-path).\n";
              return NULL;
          }
          if(!validateParams()){
                return NULL;
          }
        /*  cmd_mkdisk *mkd =  new cmd_mkdisk(size,path);
          if(unit!=UNIT_ERROR){
              mkd->unit = unit;
          }
          if(fit!=FIT_ERROR){
              mkd->fit = fit;
          }
          return (Cmd*)mkd;*/
      }
          break;
      case rmdisk:
      {
          if(path!=NULL){
              std::cout<<"Debe indicar una ruta(-path).\n";
              return NULL;
          }
          if(!validateParams()){
                return NULL;
          }
          //return cmd_rmdisk(path);
      }
          break;
      case fdisk:
      {
          if(!existSize && typePartition == -1){
              std::cout<<"Debe indicar un tamaño(-size).\n";
              return NULL;
          }
          if(path!=NULL){
              std::cout<<"Debe indicar una ruta(-path).\n";
              return NULL;
          }
          if(name!=NULL){
              std::cout<<"Debe indicar un nombre(-name).\n";
              return NULL;
          }
          if(!validateParams()){
                return NULL;
          }
          if(typeFormat!=TF_ERROR){
              //eliminar particion
             // return new cmd_rmPart(name,path,typeFormat);
          }
          if(existAdd){
              //modificar tamaño de particion
             /* cmd_addPart *ap= new cmd_addPart(name,path,size);
              if(unit!=NULL){
                  ap->unit = unit;
              }
              return ap;*/
          }
          /*cmd_fdisk *fd =  new cmd_fdisk(name,path,size);
          if(fit!=FIT_ERROR){
              fd->fit = fit;
          }
          if(typePartition!=TP_ERROR){
              fd->type = typePartition;
          }
          if(unit!=UNIT_ERROR){
              fd->unit = unit;
          }
          return fd;*/
      }
          break;
      case mount:
      {
          if(path!=NULL){
              std::cout<<"Debe indicar una ruta(-path).\n";
              return NULL;
          }
          if(name!=NULL){
              std::cout<<"Debe indicar un nombre(-name).\n";
              return NULL;
          }
          if(!validateParams()){
                return NULL;
          }
          //return new cmd_mount(path,name);
      }
          break;
      case unmount:
      {
          if(id!=NULL){
              std::cout<<"Debe indicar un id(-id).\n";
              return NULL;
          }
          if(!validateParams()){
                return NULL;
          }
          //return new cmd_unmount(id);
      }
          break;
      case exec:
      case mkfs:
      case login:
      case mkgrp:
      case rmgrp:
      case mkusr:
      case rmusr:
      case ch_mod:
      case mkfile:
      case cat:
      case rem:
      case edit:
      case ren:
      case mk_dir:
      case cp:
      case mv:
      case find_:
      case ch_own:
      case chgrp:
      case loss:
          break;
      case rep:
          if(name==NULL){
              cout<<"Error: Debe ingresar un nombre(-name)\n";
              return NULL;
          }
          if(path==NULL){
              cout<<"Error: Debe ingresar una ruta(-path)\n";
              return NULL;
          }
          if(id==NULL){
              cout<<"Error: Debe ingresar un id(-id)\n";
              return NULL;
          }
          if(!validateParams()){
                return NULL;
          }
          TypeReport typeReport;
          if(strcmp(name,"mbr")==0){
                          typeReport = Mbr;
                      }else if(strcmp(name,"disk")==0){
                          typeReport = Disk;
                      }else if(strcmp(name,"inode")==0){
                          typeReport = Inode;
                      }else if(strcmp(name,"journaling")==0){
                          typeReport = Journaling;
                      }else if(strcmp(name,"block")==0){
                          typeReport = Block;
                      }else if(strcmp(name,"bm_inode")==0){
                          typeReport = BM_inode;
                      }else if(strcmp(name,"bm_block")==0){
                          typeReport = BM_block;
                      }else if(strcmp(name,"tree")==0){
                          typeReport = Tree;
                      }else if(strcmp(name,"sb")==0){
                          typeReport = Sb;
                      }else if(strcmp(name,"file")==0){
                          typeReport = File;
                      }else if(strcmp(name,"ls")==0){
                          typeReport = Ls;
                      }else{
                          cout<<"ERROR: El tipo de reporte no es permitido.\n";
                          return NULL;
                      }
          return new cmd_rep(path,typeReport,id);
          break;

      }

}

bool validateParams(){
    return true;
}

Cmd* ListCommand(Cmd *cmd,bool flag){
    static Cmd *commandList;
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
                    ListCommand(NULL,true);
                }
            }
            fclose(input);
}

void letsExecCommands(Cmd *commands){
    Cmd *first = commands;
    while(first!=NULL){
        first->Exec();
        first = first->next;
    }
}

