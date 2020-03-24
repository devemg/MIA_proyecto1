#ifndef HANDLER_COMMANDS_H
#define HANDLER_COMMANDS_H
#include <iostream>
#include <enums.h>
#include <string.h>
#include <handler_commands.h>
#include <parser.h>
#include <scanner.h>
#include <handler_disk.h>
#include <handler_partitions.h>
#include <utils.h>

using namespace std;

class Cmd{
public:
    virtual void Exec(){}
    Cmd *next = NULL;
};

class cmd_mkdisk:public Cmd{
public:
    int size;
    Fit fit;
    Unit unit;
    char *path;
    cmd_mkdisk(int size,char path[]);
    void Exec();
};

class cmd_rmdisk:public Cmd{
public:
    char *path;
    cmd_rmdisk(char path[]);
    void Exec();
};

class cmd_fdisk:public Cmd{
public:
    int size;
    Unit unit;
    char *path;
    TypePartition type;
    Fit fit;
    char *name;
    cmd_fdisk(char name[],char path[],int size);
    void Exec();
};

class cmd_addPart:public Cmd{
public:
    int size;
    Unit unit;
    char *path;
    char *name;
    cmd_addPart(char name[],char path[],int size);
    void Exec();
};


class cmd_rmPart:public Cmd{
public:
    char *name;
    char *path;
    TypeFormat typeDel;
    TypePartition typePart;
    cmd_rmPart(char name[],char path[],TypePartition typePart,TypeFormat type);
    void Exec();
};

class cmd_mount:public Cmd{
public:
    char *path;
    char *name;
    cmd_mount(char path[],char name[]);
    void Exec();
};

class cmd_unmount:public Cmd{
public:
    char *id;
    cmd_unmount(char id[]);
    void Exec();
};

class cmd_exec:public Cmd{
public:
    char *path;
    cmd_exec(char path[]);
    void Exec();
};

//**********************************************

class cmd_fs:public Cmd{
public:
    TypeFormat type;
    FileSistem fs;
    char *id;
    cmd_fs(char id[]);
    void Exec();
};

class cmd_login:public Cmd{
public:
    char *usr;
    char *pwd;
    char *id;
    cmd_login(char usr[],char pwd[],char id[]);
    void Exec();
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

class cmd_mkfile: public Cmd{
public:
     char *path;
     bool isRecursive;
     int size;
     char *cont;
     cmd_mkfile(char path[]);
     void Exec();
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

class cmd_rep:public Cmd{
public:
    char *path_report;
    TypeReport type;
    char *id;
    char *path;
    cmd_rep(char path[],TypeReport type,char id[]);
    void Exec();
};

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

Cmd* getFormedCommand(CommandEnum command,Option *op,Cmd *cmd);

bool validateParams();

Cmd* ListCommand(Cmd *cmd,bool flag);

void writeCommand(string command,bool isScript);

void readExecCommand(bool isScript);

void letsExecCommands(Cmd *commands);

#endif // HANDLER_COMMANDS_H
