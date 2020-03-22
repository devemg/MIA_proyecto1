#ifndef HANDLER_COMMANDS_H
#define HANDLER_COMMANDS_H
#include <iostream>
#include <enums.h>
#include <string.h>

class cmd_mkdisk{
public:
    int size;
    Fit fit;
    Unit unit;
    char *path;
    cmd_mkdisk(int size,char path[]);
};

class cmd_rmdisk{
public:
    char *path;
    cmd_rmdisk(char path[]);
};

class cmd_fdisk{
public:
    int size;
    Unit unit;
    char *path;
    TypePartition type;
    Fit fit;
    char *name;
    bool isForAdd;
    cmd_fdisk(char name[],char path[],int size,bool isForAdd);
};

class cmd_rmPartition{
public:
    char *name;
    char *path;
    TypeFormat typeDel;
    cmd_rmPartition(char name[],char path[],TypeFormat type);
};

class cmd_mount{
public:
    char *path;
    char *name;
    cmd_mount(char path[],char name[]);
};

class cmd_unmount{
public:
    char *id;
    cmd_unmount(char id[]);
};

class cmd_fs{
public:
    TypeFormat type;
    FileSistem fs;
    char *id;
    cmd_fs(char id[]);
};

class cmd_login{
public:
    char *usr;
    char *pwd;
    char *id;
    cmd_login(char usr[],char pwd[],char id[]);
};

class cmd_grp{
public:
    char *name;
    bool isForCreate;
    cmd_grp(char name[],bool isForCreate);
};

class cmd_mkusr{
public:
    char *usr;
    char *pwd;
    char *grp;
    cmd_mkusr(char usr[],char pwd[],char grp[]);
};

class cmd_rmusr{
public:
    char *usr;
    cmd_rmusr(char usr[]);
};


class cmd_chmod{
public:
    char *path;
    int ugo;
    bool isRecursive;
    cmd_chmod(char path[],int ugo);
};

class cmd_mkfile{
public:
     char *path;
     bool isRecursive;
     int size;
     char *cont;
     cmd_mkfile(char path[]);
};

class cmd_file{
public:
    char *path;
    bool isForCat;//rem
    cmd_file(char path[],bool isForCat);
};

class cmd_edit{
public:
    char *path;
    char *cont;
    cmd_edit(char path[],char cont[]);
};

class cmd_ren{
public:
    char *path;
    char *newName;
    cmd_ren(char path[],char newName[]);
};


class cmd_mkdir{
public:
     char *path;
     bool isRecursive;
     cmd_mkdir(char path[]);
};

class cmd_cp{
public:
    char *path;
    char *newPath;
    bool isForCp; //isForMove
    cmd_cp(char path[],char newPath[]);
};

class cmd_find{
public:
    char *path;
    char *name;
    cmd_find(char path[],char name[]);

};

class cmd_chown{
public:
    char *path;
    char *usr;
    bool isRecursive;
    cmd_chown(char path[],char usr[]);
};

class cmd_chgrp{
public:
    char *usr;
    char *grp;
    cmd_chgrp(char usr[],char grp[]);
};

class cmd_loss{
public:
    char *id;
    cmd_loss(char id[]);
};

class cmd_rep{
public:
    char *path_report;
    TypeReport type;
    char *id;
    char *path;
    cmd_rep(char path[],TypeReport type,char id[]);
};

/*
class Node{
public:
    virtual void Exec() = 0;
};

*/
class Option
{
public:
    Option(Options op){
        option = op;
        next = NULL;
    }

    Options option;
    char text[200];
    int num;
    Fit fit;
    Unit unit;
    TypePartition type;
    TypeFormat delType;
    FileSistem fs;
    Option *next;
    bool flag;
};
/*
class Command:public Node {
public:
    CommandEnum cmd;
    Option *opts;
    Command *next;
    Command(CommandEnum c,Option *o){
        cmd = c;
        opts = o;
        next = NULL;
    }
};
*/

#endif // HANDLER_COMMANDS_H
