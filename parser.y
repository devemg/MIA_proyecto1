%{
#include "scanner.h"
#include <iostream>
#include <string.h>
#include <enums.h>
#include <utils.h>
#include <structures.h>
#include <handler_commands.h>

extern int yylineno; //linea actual
extern int columna; //columna actual
extern char *yytext; //lexema actual

int yyerror(const char* mens){
//metodo que se llama al haber un error sintactico
//SE IMPRIME EN CONSOLA EL ERROR

    if(strcmp(yytext," ")==1){
        std::cout<<"ERROR EN COMANDO: "<<mens<<" "<<yytext<< std::endl;
    }
return 0;
}

%}

%error-verbose

%union{
#include <enums.h>

//se especifican los tipo de valores para los no terminales y lo terminales
char TEXT [256];
int INT;
Fit FIT;
TypePartition TYPEPARTITION;
TypeFormat FORMATTYPE;
Unit UNIT;
CommandEnum COMMAND_ENUM;
struct Option * OPTION;
class Cmd *COMMAND;
FileSistem FILESYSTEM;
}

//TERMINALES DE TIPO TEXT, SON STRINGS
%token<TEXT> WORD
%token<TEXT> NUMERO
%token<TEXT> SIZE
%token<TEXT> FIT
%token<TEXT> UNIT
%token<TEXT>  PATH
%token<TEXT>  TYPE
%token<TEXT>  DELETE
%token<TEXT>  NAME
%token<TEXT>  ADD
%token<TEXT>  ID
%token<TEXT>  MKDISK
%token<TEXT>  RMDISK
%token<TEXT>  FDISK
%token<TEXT>  MOUNT
%token<TEXT>  UNMOUNT
%token<TEXT>  REP
%token<TEXT>  EXEC
%token<TEXT>  IGUAL
%token<TEXT>  BF
%token<TEXT>  WF
%token<TEXT>  FF
%token<TEXT>  B_
%token<TEXT>  K_B
%token<TEXT>  M_B
%token<TEXT>  P_
%token<TEXT>  E_
%token<TEXT>  L_
%token<TEXT>  FAST
%token<TEXT>  FULL
%token<TEXT>  PATH_

%token<TEXT>  FS
%token<TEXT>  EXT2
%token<TEXT>  EXT3

%token<TEXT>  USR
%token<TEXT>  PWD
%token<TEXT>  GRP
%token<TEXT>  UGO
%token<TEXT>  RECURSIVE
%token<TEXT>  CONTENT
%token<TEXT>  _FILE
%token<TEXT>  DESTINY
%token<TEXT>  RUTA

%token<TEXT>  LOSS
%token<TEXT>  CHGRP
%token<TEXT>  CHOWN
%token<TEXT>  FIND
%token<TEXT>  MV
%token<TEXT>  CP
%token<TEXT>  MKDIR
%token<TEXT>  REN
%token<TEXT>  EDIT
%token<TEXT>  REM
%token<TEXT>  CAT
%token<TEXT>  MKFILE
%token<TEXT>  CHMOD
%token<TEXT>  RMUSR
%token<TEXT>  MKUSR
%token<TEXT>  RMGRP
%token<TEXT>  LOGIN
%token<TEXT>  MKFS
%token<TEXT>  MKGRP
%token<TEXT>  RECOVERY
%token<TEXT>  LOGOUT
%token<TEXT>  PAUSE

//NO TERMINALES DE TIPO VAL, POSEEN ATRIBUTOS INT VALOR, Y QSTRING TEXTO
%type<TEXT>  INICIO
%type<FIT>  FIT_OPTIONS
%type<TEXT>  STRING
%type<TYPEPARTITION>  TYPE_OPTIONS
%type<FORMATTYPE>  DELETE_OPTIONS
%type<UNIT>  UNIT_OPTIONS

%type<OPTION>  OPTIONS_LIST
%type<OPTION>  STATE_OPTION

%type<COMMAND_ENUM>  STATE_COMMANDS
%type<COMMAND>  COMMANDS_LIST
%type<COMMAND>  COMMAND
%type<FILESYSTEM> FS_OPTIONS

%start INICIO

%%

INICIO :COMMANDS_LIST{
ListCommand($1,true);
};

COMMANDS_LIST: COMMANDS_LIST COMMAND{
    if($1!=NULL){
        if($2!=NULL){
            Cmd *first = $1;
            while(first->next!=NULL){
                first = first->next;
            }
            first->next = $2;
        }
    $$=$1;
    }else{
        if($2!=NULL){
            $$=$2;
        }else{
            $$=NULL;
        }
    }
}
|COMMAND{
   $$ = $1;
};

COMMAND: STATE_COMMANDS OPTIONS_LIST{
    Cmd *cmd = getFormedCommand($1,$2);
    $$ = cmd;
}
|PAUSE{$$=new cmd_pause();}
|LOGOUT{$$=new cmd_logout();};

STATE_COMMANDS: MKDISK{$$=mkdisk;}
|RMDISK{$$=rmdisk;}
|FDISK{$$=fdisk;}
|MOUNT{$$=mount;}
|UNMOUNT{$$=unmount;}
|REP{$$=rep;}
|EXEC{$$=exec;}
|LOSS{$$=loss;}
|CHGRP{$$=chgrp;}
|CHOWN{$$=ch_own;}
|FIND{$$=find_;}
|MV{$$=mv;}
|CP{$$=cp;}
|MKDIR{$$=mk_dir;}
|REN{$$=ren;}
|EDIT{$$=edit;}
|REM{$$=rem;}
|CAT{$$=cat;}
|MKFILE{$$=mkfile;}
|CHMOD{$$=ch_mod;}
|RMUSR{$$=rmusr;}
|MKUSR{$$=mkusr;}
|LOGIN{$$=login;}
|MKFS{$$=mkfs;}
|MKGRP{$$=mkgrp;}
|RECOVERY{$$=recovery;}
|RMGRP{$$=rmgrp;}
;


OPTIONS_LIST:OPTIONS_LIST STATE_OPTION{
Option *op = $1;
while(op->next!=NULL){
    op = op->next;
}
op->next = $2;
$$ = $1;
}
|STATE_OPTION{
    $$ = $1;
};


STATE_OPTION:NAME IGUAL WORD{
    $$ = new Option(Name);
    strcpy($$->text, $3);
}
|SIZE IGUAL NUMERO{
    $$ = new Option(Size);
    $$->num = getInt($3);
}
|FIT IGUAL FIT_OPTIONS{
    $$ = new Option(Fitt);
    $$->fit = $3;
}
|UNIT IGUAL UNIT_OPTIONS{
    $$ = new Option(Unitt);
    $$->unit = $3;
}
|PATH IGUAL STRING{
    $$ = new Option(Path);
    strcpy($$->text, $3);
}
|TYPE IGUAL TYPE_OPTIONS{
    $$ = new Option(Type);
    $$->type = $3;
}
|DELETE  IGUAL DELETE_OPTIONS{
    $$ = new Option(Delete);
    $$->delType = $3;
}
|ADD IGUAL NUMERO{
    $$ = new Option(Add);
    $$->num = getInt($3);
}
|ID IGUAL WORD{
     $$ = new Option(Id);
     strcpy($$->text, $3);
}
|TYPE IGUAL DELETE_OPTIONS{
    $$ = new Option(Format);
    $$->delType = $3;
}
|FS IGUAL FS_OPTIONS{
    $$ = new Option(FileSys);
    $$->fs = $3;
}
|USR IGUAL STRING{
    $$ = new Option(Usr);
    strcpy($$->text, $3);
}
|PWD IGUAL STRING{
    $$ = new Option(Pwd);
    strcpy($$->text, $3);
}
|PWD IGUAL NUMERO{
    $$ = new Option(Pwd);
    strcpy($$->text,$3);
}
|GRP IGUAL STRING{
    $$ = new Option(Grp);
    strcpy($$->text, $3);
}
|UGO IGUAL NUMERO{
    $$ = new Option(Ugo);
    $$->num = getInt($3);
}
|RECURSIVE{
    $$ = new Option(Recursive);
    $$->flag = true;
}
|CONTENT IGUAL STRING{
    $$ = new Option(Cont);
    strcpy($$->text, $3);
}
|_FILE IGUAL STRING{
    $$ = new Option(Fil_e);
    strcpy($$->text, $3);
}
|DESTINY IGUAL STRING{
    $$ = new Option(Dest);
    strcpy($$->text, $3);
}
|RUTA IGUAL STRING{
    $$ = new Option(Ruta);
    strcpy($$->text, $3);
}

;

STRING:PATH_{strcpy($$,$1);}|WORD{strcpy($$,$1);};

FS_OPTIONS:EXT2{$$=ext2;}|EXT3{$$=ext3;};

DELETE_OPTIONS:FAST{$$=Fast;}|FULL{$$=Full;};

TYPE_OPTIONS:P_{$$=Primaria;}|E_{$$=Extendida;}|L_{$$=Logica;};

UNIT_OPTIONS:K_B{$$=KB;}|M_B{$$=MB;}|B_{$$=Byte;};

FIT_OPTIONS:BF{$$=BestFit;}|FF{$$=FirstFit;}|WF{$$=WorstFit;};

%%
