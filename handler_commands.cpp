#include "handler_commands.h"

cmd_mkdisk::cmd_mkdisk(int size, char path[]){
    this->size = size;
    this->path = path;
    this->fit = FirstFit;
    this->unit = MB;
}

//CREAR DISCO
void cmd_mkdisk::Exec(){
    int ext = 0;
    string ss;
    ss = getNamePath(path,&ext);
    char *chh = &ss[0];
    string hh = getPathWithoutName(path,strlen(chh)+ext);
    Response res = newDisk(size,fit,unit,&hh[0],chh);
    if(res==SUCCESS){
        cout<<"¡Disco creado con éxito!\n";

    }else{
        showMessageError(res);
    }
}

cmd_rmdisk::cmd_rmdisk(char path[]){
    this->path = path;
}

//ELIMINAR DISCO
void cmd_rmdisk::Exec(){
    deleteDisk(path);
}

cmd_fdisk::cmd_fdisk(char name[], char path[], int size){
    this->name = name;
    this->path = path;
    this->size = size;
}

//CREAR PARTICION
void cmd_fdisk::Exec(){
    int ext = 0;
    string ss;
    ss = getNamePath(path,&ext);
    char *chh = &ss[0];
    string hh = getPathWithoutName(path,strlen(chh)+ext);

    Response res =  createPartition(size,unit,&hh[0],chh,this->type,fit,name);
    if(res == SUCCESS){
        switch (this->type) {
        case Primaria:
            cout<<"¡La partición primaria fue creada con éxito!\n";
            break;
        case Extendida:
            cout<<"¡La partición extendida fue creada con éxito!\n";
            break;
        case Logica:
            cout<<"¡La partición lógica fue creada con éxito!\n";
            break;
        default:
            break;
        }
    }else{
        showMessageError(res);
    }
}

cmd_addPart::cmd_addPart(char name[], char path[], int size){
    this->size = size;
    this->name = name;
    this->path = path;
    this->unit = KB;
}

//MODIFICAR PARTICION
void cmd_addPart::Exec(){
    int ext = 0;
    string ss;
    ss = getNamePath(path,&ext);
    char *chh = &ss[0];
    string hh = getPathWithoutName(path,strlen(chh)+ext);
    Response res = updatepartition(&hh[0],chh,name,this->size,unit);
    if(res == SUCCESS){
        cout<<"¡La partición fue modificada cón éxito!\n";
    }else{
        showMessageError(res);
    }
}

cmd_rmPart::cmd_rmPart(char name[], char path[],TypePartition typePart,TypeFormat type){
    this->name = name;
    this->path = path;
    this->typeDel = type;
    this->typePart = typePart;
}

//ELIMINAR PARTICION
void cmd_rmPart::Exec(){
    int ext = 0;
    string ss;
    ss = getNamePath(path,&ext);
    char *chh = &ss[0];
    string hh = getPathWithoutName(path,strlen(chh)+ext);
    Response res = deletePartition(&hh[0],chh,name,this->typePart,this->typeDel);
    if(res==SUCCESS){
        cout<<"¡Partición eliminada con éxito!\n";
    }else{
        showMessageError(res);
    }
}

cmd_mount::cmd_mount(char path[], char name[]){
    this->path = path;
    this->name = name;
}

//MONTAR PARTICION
void cmd_mount::Exec(){
    Response res = mountPart(path,name);
    if(res!=SUCCESS){
        showMessageError(res);
        return;
    }else{
        cout<<"¡Partición montada con éxito!\n";
    }
}

cmd_unmount::cmd_unmount(char id[]){
    this->id = id;
}

//DESMONTAR PARTICION
void cmd_unmount::Exec(){
     Response res = unmountPart(id);
    if(res!=SUCCESS){
        showMessageError(res);
        return;
    }else{
        showMounts();
        cout<<"¡Partición desmontada con éxito!\n";
    }
}

cmd_exec::cmd_exec(char path[]){
    this->path = path;
}

//EJECUTAR SCRIPT
void cmd_exec::Exec(){
cout<<"Ejecutando script...\n\n";
    FILE *myFile = fopen(path,"r");
    if(myFile==NULL){
        cout<<"Error al abrir el script.\n";
        return;
    }
    char linea[1024];
    while(fgets(linea, 1024, (FILE*) myFile)) {
            cout<<linea<<endl;
            writeCommand(linea,true);
            readExecCommand(true);
        }
   fclose(myFile);
}

//**********************************************

cmd_fs::cmd_fs(char id[]){
    type = Full;
    this->id = id;
    fs = ext2;
}

//FILE-SYSTEM DE PARTICION
void cmd_fs::Exec(){
    MountedDisk *disk = getMountedDisk(this->id);
    if(disk==NULL){
        cout<<"El disco no ha sido montado.\n";
    }
    MountedPart *part = getMountedPartition(this->id);
    if(part==NULL){
        cout<<"la partición no fue encontrada.\n";
    }
    Response res = formatPart(disk->path,part->name,this->type,this->fs,false);
    if(res==SUCCESS){
        cout<<"¡Sistema de archivos creado con éxito!\n";
    }else{
        showMessageError(res);
    }
}

cmd_login::cmd_login(char usr[], char pwd[], char id[]){
    this->usr = usr;
    this->pwd = pwd;
    this->id = id;
}

//INICIAR SESION
void cmd_login::Exec(){
    if(isSesionActive()){
        cout<<"El usuario "<<active_sesion->user<<" ya ha iniciado sesión.\n";
    }else{
        MountedDisk *disk = getMountedDisk(this->id);
        if(disk==NULL){
            return;
        }
        MountedPart *part = getMountedPartition(this->id);
        if(part==NULL){
            return;
        }
        //************************************************
        //COMPROBAR SESION
        User *user = getUser(this->usr,disk->path,part->name);
        if(user==NULL){
            showMessageError(ERROR_USER_NOT_EXISTS);
            return;
        }
        if(user->pwd != this->pwd){
            showMessageError(ERROR_BAD_PASSWD);
            return;
        }

        //************************************************
        int check = 0; //loginInSystem(usr,pwd);
        if(check == 0){
            active_sesion->user = &user->name[0];
            active_sesion->path = disk->path;
            active_sesion->namePartition = part->name;
            active_sesion->id = this->id;
            active_sesion->idUser = &user->id[0];
            active_sesion->idGrp = &user->group[0];
            cout<<"¡Sesión iniciada!\n";
        }else if(check == 1){
            cout<<"El usuario no existe\n";
        }else{
            cout<<"Las credenciales son incorrectas.\n";
        }
    }
}

cmd_grp::cmd_grp(char name[], bool isForCreate){
    this->name = name;
    this->isForCreate = isForCreate;
}

//CREAR-ELIMINAR GRUPOS
void cmd_grp::Exec(){
    if(!isSesionActive()){
        cout<<"No se ha iniciado sesión.\n";
        return;
    }
    if(isForCreate){
        if(strcmp(active_sesion->user,"root")!=0){
            cout<<"Solo el usuario root puede crear grupos.\n";
            return;
        }

        Response res = addGroup(active_sesion->path,active_sesion->namePartition,this->name);
        if(res==SUCCESS){
            cout<<"¡Grupo creado con éxito!\n";
        }else{
            showMessageError(res);
        }
    }else{
        Response res = deleteGroup(active_sesion->path,active_sesion->namePartition,this->name);
        if(res==SUCCESS){
            cout<<"¡Grupo eliminado con éxito!\n";
        }else{
            showMessageError(res);
        }
    }
}

cmd_mkusr::cmd_mkusr(char usr[], char pwd[], char grp[]){
    this->usr = usr;
    this->pwd = pwd;
    this->grp = grp;
}

//CREAR USUARIOS
void cmd_mkusr::Exec(){
    if(!isSesionActive()){
        cout<<"No se ha iniciado sesión.\n";
        return;
    }
    Response res = addUser(active_sesion->path,active_sesion->namePartition,this->usr,this->pwd,this->grp);
    if(res==SUCCESS){
        cout<<"¡Grupo creado con éxito!\n";
    }else{
        showMessageError(res);
    }
}

cmd_rmusr::cmd_rmusr(char usr[]){
    this->usr = usr;
}

//ELIMINAR USUARIOS
void cmd_rmusr::Exec(){
    if(!isSesionActive()){
        cout<<"No se ha iniciado sesión.\n";
        return;
    }
    Response res = deleteGroup(active_sesion->path,active_sesion->namePartition,this->usr);
    if(res==SUCCESS){
        cout<<"¡Usuario eliminado con éxito!\n";
    }else{
        showMessageError(res);
    }
}

cmd_chmod::cmd_chmod(char path[], int ugo){
    this->path = path;
    this->ugo = ugo;
    this->isRecursive = false;
}

void cmd_chmod::Exec(){}

cmd_mkfile::cmd_mkfile(char path[]){
    this->path = path;
    this->isRecursive = false;
    this->size = 0;
    this->cont = NULL;
}

//CREAR ARCHIVO
void cmd_mkfile::Exec(){
    if(!isSesionActive()){
        cout<<"No se ha iniciado sesión.\n";
        return;
    }
    if(cont!=NULL){
       Response res = createFile (this->path,this->isRecursive,this->cont,
                                  active_sesion->path,active_sesion->namePartition);
       if(res==SUCCESS){
           cout<<"¡Archivo creado con éxito!\n";
       }else{
           showMessageError(res);
       }
    }else{
       Response res = createFile(this->path,this->isRecursive,this->size,
                 active_sesion->path,active_sesion->namePartition);
       if(res==SUCCESS){
           cout<<"¡Archivo creado con éxito!\n";
       }else{
           showMessageError(res);
       }
    }
}

cmd_file::cmd_file(char path[], bool isForCat){
    this->path = path;
    this->isForCat = isForCat;
}

//MOSTRAR ARCHIVO
void cmd_file::Exec(){
    if(!isSesionActive()){
        cout<<"No se ha iniciado sesión.\n";
        return;
    }
    if(isForCat){
        Response res = catFile(this->path,active_sesion->path,active_sesion->namePartition);
        if(res!=SUCCESS){
            showMessageError(res);
        }
    }else{
        //rem
    }
}

cmd_edit::cmd_edit(char path[], char cont[]){
    this->path = path;
    this->cont = cont;
}

//EDITAR ARCHIVO
void cmd_edit::Exec(){
    if(!isSesionActive()){
        cout<<"No se ha iniciado sesión.\n";
        return;
    }
    Response res = editFile(this->path,this->cont,active_sesion->path,active_sesion->namePartition);
    if(res==SUCCESS){
        cout<<"¡Archivo creado con éxito!\n";
    }else{
        showMessageError(res);
    }
}

cmd_ren::cmd_ren(char path[], char newName[]){
    this->path = path;
    this->newName = newName;
}

void cmd_ren::Exec(){}

cmd_mkdir::cmd_mkdir(char path[], bool isRecursive){
    this->isRecursive = isRecursive;
    this->path = path;
}

//CREAR CARPETA
void cmd_mkdir::Exec(){
    if(!isSesionActive()){
        cout<<"No se ha iniciado sesión.\n";
        return;
    }
    Response res = createDirectory(this->isRecursive,active_sesion->id,this->path);
    if(res==SUCCESS){
        cout<<"¡Carpeta creada con éxito!\n";
    }else{
        showMessageError(res);
    }
}

cmd_cp::cmd_cp(char path[], char newPath[],bool isForCp){
    this->path = path;
    this->newPath = newPath;
    this->isForCp = isForCp;
}

void cmd_cp::Exec(){}

cmd_find::cmd_find(char path[], char name[]){
    this->path = path;
    this->name = name;
}

void cmd_find::Exec(){}

cmd_chown::cmd_chown(char path[], char usr[],bool isRecursive){
    this->path = path;
    this->isRecursive = isRecursive;
    this->usr = usr;
}

void cmd_chown::Exec(){}

cmd_chgrp::cmd_chgrp(char usr[], char grp[]){
    this->usr = usr;
    this->grp = grp;
}

void cmd_chgrp::Exec(){}

cmd_loss::cmd_loss(char id[]){
    this->id = id;
}

//LIMPIAR
void cmd_loss::Exec(){
    MountedDisk *disk = getMountedDisk(this->id);
    if(disk==NULL){
        cout<<"El disco no ha sido montado.\n";
        return;
    }
    MountedPart *part = getMountedPartition(this->id);
    if(part==NULL){
        cout<<"la partición no fue encontrada.\n";
        return;
    }

    Response res =  clearAllSystem(disk->path,part->name);
    if(res == SUCCESS){
        cout<<"¡El sistema de archivos ha sido corrompido!\n";
    }else{
        showMessageError(res);
    }
}

cmd_rep::cmd_rep(char path[], TypeReport type, char id[]){
    this->path_report = path;
    this->type = type;
    this->id = id;
    this->path = NULL;
}

//GENERAR REPORTE
void cmd_rep::Exec(){

    MountedDisk *disk = getMountedDisk(this->id);
    if(disk==NULL){
        cout<<"No hay un disco montado.\n";
        return;
    }
    MountedPart *part = getMountedPartition(this->id);
    if(part==NULL){
        cout<<"La partición no fue montada.\n";
        return;
    }

    int ext = 0;
    string ss;
    ss = getNamePath(this->path_report,&ext);
    char *chh = &ss[0];
    string hh = getPathWithoutName(this->path_report,strlen(chh)+ext);

    //cout<<"PATH: "<<hh<<endl;
    //CREAR DIRECTORIO SI NO EXISTE
    mkdir(hh.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    switch (this->type) {
    case Mbr:
        reportMBR(disk->path,this->path_report);
        break;
    case Disk:
        reportDisk(disk->path,this->path_report);
        break;
    case Inode:
    {
        Response res = reportInodes(disk->path,part->name, this->path_report);
        if(res==SUCCESS){
            cout<<"El reporte fue generado con éxito.\n";
        }else{
            showMessageError(res);
        }
    }
        break;
    case Block:
    {
        Response res =reportBlocks(disk->path,part->name, this->path_report);
        if(res==SUCCESS){
            cout<<"El reporte fue generado con éxito.\n";
        }else{
            showMessageError(res);
        }
    }
        break;
    case Journaling:
    {
        Response res =reportJournal(disk->path,part->name,this->path_report);
        if(res==SUCCESS){
            cout<<"El reporte fue generado con éxito.\n";
        }else{
            showMessageError(res);
        }
    }
        break;
    case BM_block:
    {
        Response res =reportBitmap(1,disk->path,part->name,this->path_report);
        if(res==SUCCESS){
            cout<<"El reporte fue generado con éxito.\n";
        }else{
            showMessageError(res);
        }
    }
        break;
    case BM_inode:
    {
        Response res =reportBitmap(0,disk->path,part->name,this->path_report);
        if(res==SUCCESS){
            cout<<"El reporte fue generado con éxito.\n";
        }else{
            showMessageError(res);
        }
    }
        break;
    case Tree:
    {
        reportTree(this->path_report,this->id);
            cout<<"El reporte fue generado con éxito.\n";

    }
        break;
    case Sb:
    {
        reportSuperBlock(disk->path,part->name,this->path_report);
            cout<<"El reporte fue generado con éxito.\n";
    }
        break;
    case File:
    {
        Response res = reportFile(this->path,disk->path,part->name,this->path_report);
        if(res==SUCCESS){
            cout<<"El reporte fue generado con éxito.\n";
        }else{
            showMessageError(res);
        }
    }
        break;
    case Ls:
        break;
    default:
        cout<<"Reporte no reconocido.\n";
        break;
    }
}

cmd_recovery::cmd_recovery(char *id){
    this->id = id;
}

//RECONSTRUIR SISTEMA DE ARCHIVOS
void cmd_recovery::Exec(){
    MountedDisk *disk = getMountedDisk(this->id);
    if(disk==NULL){
        cout<<"El disco no ha sido montado.\n";
        return;
    }
    MountedPart *part = getMountedPartition(this->id);
    if(part==NULL){
        cout<<"la partición no fue encontrada.\n";
        return;
    }
    int startSb=0;
    SuperBlock *sb = readSuperBlock(disk->path,part->name,&startSb);
    if(sb==NULL){
        showMessageError(ERROR_FILESYSTEM);
        return;
    }
    if(sb->s_filesystem_type == ext2){
        showMessageError(ERROR_FILESYSTEM);
        return;
    }
    Response res = recoverySystem(sb,startSb,disk->path,part->name,this->id);
    if(res == SUCCESS){
        cout<<"¡El sistema de archivos ha sido recuperado con éxito!\n";
    }else{
        showMessageError(res);
    }
    delete sb;
}

Cmd* getFormedCommand(CommandEnum command,Option *op){
    Option *it = op;

    char *name = NULL;
    char *path = NULL;
    char *id = NULL;
    char *usr = NULL;
    char *pwd=NULL;
    char *grp=NULL;
    char *cont=NULL;
    char *file=NULL;
    char *dest=NULL;

    int size = -1;
    int ugo=-1;

    bool isRecursive=false;
    Fit fit = FIT_ERROR;
    Unit unit = UNIT_ERROR;
    TypeFormat typeFormat = TF_ERROR;
    TypePartition typePartition = TP_ERROR;
    FileSistem fsystem = FS_ERROR;

    bool existSize = false;
    bool existAdd = false;
    bool existUgo = false;

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
        case Format:
            typeFormat = it->delType;
            break;
        case Add:
            size = it->num;
            existAdd = true;
            break;
        case Usr:
            usr = it->text;
            break;
        case Pwd:
            pwd = it->text;
            break;
        case Grp:
            grp = it->text;
            break;
        case Cont:
            cont=it->text;
            break;
        case Fil_e:
            file = it->text;
            break;
        case Dest:
            dest = it->text;
            break;
        case Ugo:
            ugo = it->num;
            existUgo = true;
            break;
        case Recursive:
            isRecursive = it->flag;
            break;
        case FileSys:
            fsystem = it->fs;
            break;
        default:
            cout<<"La opción no es válida.\n";
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
          if(path==NULL){
              std::cout<<"Debe indicar una ruta(-path).\n";
              return NULL;
          }
          if(!validateParams()){
                return NULL;
          }
          cmd_mkdisk *mkd =  new cmd_mkdisk(size,path);
          if(unit!=UNIT_ERROR){
              mkd->unit = unit;
          }
          if(fit!=FIT_ERROR){
              mkd->fit = fit;
          }
          return (Cmd*)mkd;
      }
          break;
      case rmdisk:
      {
          if(path==NULL){
              std::cout<<"Debe indicar una ruta(-path).\n";
              return NULL;
          }
          if(!validateParams()){
                return NULL;
          }
          return new cmd_rmdisk(path);
      }
          break;
      case fdisk:
      {
          if(!existSize && typePartition == -1){
              std::cout<<"Debe indicar un tamaño(-size).\n";
              return NULL;
          }
          if(path==NULL){
              std::cout<<"Debe indicar una ruta(-path).\n";
              return NULL;
          }
          if(name==NULL){
              std::cout<<"Debe indicar un nombre(-name).\n";
              return NULL;
          }
          if(!validateParams()){
                return NULL;
          }
          if(typeFormat!=TF_ERROR){
              //eliminar particion
             cmd_rmPart *rp = new cmd_rmPart(name,path,Primaria,typeFormat);
              if(typePartition!=TP_ERROR){
                  rp->typePart = typePartition;
              }
              return rp;
          }
          if(existAdd){
              //modificar tamaño de particion
              cmd_addPart *ap= new cmd_addPart(name,path,size);
              if(unit!=UNIT_ERROR){
                  ap->unit = unit;
              }
              return ap;
          }
          cmd_fdisk *fd =  new cmd_fdisk(name,path,size);
          if(fit!=FIT_ERROR){
              fd->fit = fit;
          }
          if(typePartition!=TP_ERROR){
              fd->type = typePartition;
          }
          if(unit!=UNIT_ERROR){
              fd->unit = unit;
          }
          return fd;
      }
          break;
      case mount:
      {
          if(path==NULL){
              std::cout<<"Debe indicar una ruta(-path).\n";
              return NULL;
          }
          if(name==NULL){
              std::cout<<"Debe indicar un nombre(-name).\n";
              return NULL;
          }
          if(!validateParams()){
                return NULL;
          }
          return new cmd_mount(path,name);
      }
          break;
      case unmount:
      {
          if(id==NULL){
              std::cout<<"Debe indicar un id(-id).\n";
              return NULL;
          }
          if(!validateParams()){
                return NULL;
          }
          return new cmd_unmount(id);
      }
          break;
      case exec:
      {
          if(path==NULL){
              std::cout<<"Debe indicar una ruta(-path).\n";
              return NULL;
          }
          return new cmd_exec(path);
      }
          break;
      case mkfs:
      {
          if(id==NULL){
              std::cout<<"Debe indicar un id(-id).\n";
              return NULL;
          }
          cmd_fs *fs = new cmd_fs(id);
          if(typeFormat!=TF_ERROR){
              fs->type = typeFormat;
          }
          if(fsystem!=FS_ERROR){
              fs->fs = fsystem;
          }
          return fs;
      }
          break;
      case login:
      {
          if(usr==NULL){
              std::cout<<"Debe indicar un usr(-usr).\n";
              return NULL;
          }
          if(pwd==NULL){
              std::cout<<"Debe indicar un pwd(-pwd).\n";
              return NULL;
          }
          if(id==NULL){
              std::cout<<"Debe indicar un id(-id).\n";
              return NULL;
          }
          return new cmd_login(usr,pwd,id);
      }
          break;
      case mkgrp:
      {
          if(name==NULL){
              std::cout<<"Debe indicar un name(-name).\n";
              return NULL;
          }
          return new cmd_grp(name,true);
      }
          break;
      case rmgrp:
      {
          if(name==NULL){
              std::cout<<"Debe indicar un name(-name).\n";
              return NULL;
          }
          return new cmd_grp(name,false);
      }
          break;
      case mkusr:
      {
          if(usr==NULL){
              std::cout<<"Debe indicar un usr(-usr).\n";
              return NULL;
          }
          if(pwd==NULL){
              std::cout<<"Debe indicar un pwd(-pwd).\n";
              return NULL;
          }
          if(grp==NULL){
              std::cout<<"Debe indicar un grp(-grp).\n";
              return NULL;
          }
          return new cmd_mkusr(usr,pwd,grp);
      }
          break;
      case rmusr:
      {
          if(usr==NULL){
              std::cout<<"Debe indicar un usr(-usr).\n";
              return NULL;
          }
          return new cmd_rmusr(usr);
      }
          break;
      case ch_mod:
      {
          if(path==NULL){
              std::cout<<"Debe indicar un path(-path).\n";
              return NULL;
          }
          if(!existUgo){
              std::cout<<"Debe indicar los permisos(-ugo).\n";
              return NULL;
          }
          cmd_chmod *cmod = new cmd_chmod(path,ugo);
          cmod->isRecursive = isRecursive;
          return cmod;
      }
          break;
      case mkfile:
      {
          if(path==NULL){
              std::cout<<"Debe indicar un path(-path).\n";
              return NULL;
          }
          if(!existSize && cont == NULL ){
              std::cout<<"Debe indicar un tamaño(-size) o la ruta de un archivo(-cont).\n";
              return NULL;
          }
          cmd_mkfile *mk = new cmd_mkfile(path);
          mk->isRecursive = isRecursive;
          if(existSize){
              mk->size = size;
          }
          if(cont !=NULL){
              mk->cont = cont;
          }
          return mk;
      }
          break;
      case cat:
      {
          if(file==NULL){
              std::cout<<"Debe indicar una ruta de archivo(-file).\n";
              return NULL;
          }
          return new cmd_file(file,true);
      }
          break;
      case rem:
      {
          if(path==NULL){
              std::cout<<"Debe indicar un path(-path).\n";
              return NULL;
          }
          return new cmd_file(path,false);
      }
          break;
      case edit:
      {
          if(path==NULL){
              std::cout<<"Debe indicar un path(-path).\n";
              return NULL;
          }
          if(cont==NULL){
              std::cout<<"Debe indicar una ruta de archivo(-cont).\n";
              return NULL;
          }
          return new cmd_edit(path,cont);
      }
          break;
      case ren:
      {
          if(path==NULL){
              std::cout<<"Debe indicar un path(-path).\n";
              return NULL;
          }
          if(name==NULL){
              std::cout<<"Debe indicar un name(-name).\n";
              return NULL;
          }
         return new cmd_ren(path,name);
      }
          break;
      case mk_dir:
      {
          if(path==NULL){
              std::cout<<"Debe indicar un path(-path).\n";
              return NULL;
          }
          return new cmd_mkdir(path,isRecursive);
      }
          break;
      case cp:
      {
          if(path==NULL){
              std::cout<<"Debe indicar un path(-path).\n";
              return NULL;
          }
          if(dest==NULL){
              std::cout<<"Debe indicar un dest(-dest).\n";
              return NULL;
          }
          return new cmd_cp(path,dest,true);
      }
          break;
      case mv:
      {
          if(path==NULL){
              std::cout<<"Debe indicar un path(-path).\n";
              return NULL;
          }
          if(dest==NULL){
              std::cout<<"Debe indicar un dest(-dest).\n";
              return NULL;
          }
          return new cmd_cp(path,dest,false);
      }
          break;
      case find_:
      {
          if(path==NULL){
              std::cout<<"Debe indicar un path(-path).\n";
              return NULL;
          }
          if(name==NULL){
              std::cout<<"Debe indicar un nombre(-name).\n";
              return NULL;
          }
          return new cmd_find(path,name);
      }
          break;
      case ch_own:
      {
          if(path==NULL){
              std::cout<<"Debe indicar una ruta(-path).\n";
              return NULL;
          }
          if(usr==NULL){
              std::cout<<"Debe indicar un usuario(-usr).\n";
              return NULL;
          }
          return new cmd_chown(path,usr,isRecursive);
      }
          break;
      case chgrp:
      {
          if(usr==NULL){
              std::cout<<"Debe indicar un usuario(-usr).\n";
              return NULL;
          }
          if(grp==NULL){
              std::cout<<"Debe indicar un grupo(-grp).\n";
              return NULL;
          }
          return new cmd_chgrp(usr,grp);
      }
          break;
      case loss:
      {
          if(id==NULL){
              std::cout<<"Debe indicar un id(-id).\n";
              return NULL;
          }
          return new cmd_loss(id);
      }
          break;
      case recovery:
      {
          if(id== NULL){
              std::cout<<"Debe indicar un id(-id).\n";
              return NULL;
          }

          return new cmd_recovery(id);
      }
          break;
      case rep:
      {
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
      }
          break;
      }
 return NULL;
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

bool isSesionActive(){
    if(active_sesion->user!=NULL){
        //cout<<active_sesion->user<<endl;
        return true;
    }
    if(active_sesion->path!=NULL){
        //cout<<active_sesion->path<<endl;
        return true;
    }
    if( active_sesion->namePartition!=NULL){
        //cout<<active_sesion->namePartition<<endl;
        return true;
    }
    if(active_sesion->id!=NULL){
        //cout<<active_sesion->id<<endl;
        return true;
    }
    return false;
}
