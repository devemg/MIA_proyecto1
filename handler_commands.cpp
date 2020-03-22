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

cmd_fdisk::cmd_fdisk(char name[], char path[], int size,bool isForAdd){
    this->size = size;
    this->name = name;
    this->path = path;
    this->unit = KB;
    this->type = Primaria;
    this->fit = WorstFit;
    this->isForAdd = isForAdd;
}

cmd_rmPartition::cmd_rmPartition(char name[], char path[],TypeFormat type){
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
