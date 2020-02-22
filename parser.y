%{
#include "scanner.h"
#include <iostream>
#include <string.h>
#include <enums.h>

extern int yylineno; //linea actual
extern int columna; //columna actual
extern char *yytext; //lexema actual

int yyerror(const char* mens){
//metodo que se llama al haber un error sintactico
//SE IMPRIME EN CONSOLA EL ERROR
std::cout<<"ERROR EN COMANDO: "<<mens<<" "<<yytext<< std::endl;
return 0;
}

%}

%error-verbose

%union{
#include <enums.h>

//se especifican los tipo de valores para los no terminales y lo terminales
char TEXT [256];
double VAL;
int INT;
Fit FIT;
TipoParticion TIPOPARTICION;
DeleteType DELETETYPE;
Unit UNIT;
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

//NO TERMINALES DE TIPO VAL, POSEEN ATRIBUTOS INT VALOR, Y QSTRING TEXTO
%type<VAL>  INICIO
%type<VAL>  STATE_OPTION
%type<VAL>  STATE_COMMANDS
%type<FIT>  FIT_OPTIONS
%type<TIPOPARTICION>  TYPE_OPTIONS
%type<DELETETYPE>  DELETE_OPTIONS
%type<UNIT>  UNIT_OPTIONS

%type<VAL>  OPTIONS_LIST
%type<VAL>  COMMANDS_LIST
%type<VAL>  COMMAND


%start INICIO

%%

INICIO :COMMANDS_LIST;

COMMANDS_LIST: COMMANDS_LIST COMMAND{}
|COMMAND{};

COMMAND: STATE_COMMANDS OPTIONS_LIST{

            };

STATE_COMMANDS: MKDISK{$$=0;}|RMDISK{$$=1;}|FDISK{$$=2;}|MOUNT{$$=3;}|UNMOUNT{$$=4;}|REP{$$=5;}|EXEC{$$=6;};


OPTIONS_LIST:OPTIONS_LIST STATE_OPTION
|STATE_OPTION;


STATE_OPTION:NAME IGUAL WORD{
    std::cout<<"NAME "<<$3<<std::endl;
}
|SIZE IGUAL NUMERO{
    std::cout<<"size "<<$3<<std::endl;
}
|FIT IGUAL FIT_OPTIONS{

}
|UNIT IGUAL UNIT_OPTIONS{}
|PATH IGUAL WORD{}
|TYPE IGUAL TYPE_OPTIONS{}
|DELETE  IGUAL DELETE_OPTIONS{}
|ADD IGUAL NUMERO{}
|ID IGUAL WORD{};

DELETE_OPTIONS:FAST{$$=Fast;}|FULL{$$=Full;};

TYPE_OPTIONS:P_{$$=Primaria;}|E_{$$=Extendida;}|L_{$$=Logica;};

UNIT_OPTIONS:K_B{$$=KB;}|M_B{$$=MB;}|B_{$$=Byte;};

FIT_OPTIONS:BF{$$=BestFit;}|FF{$$=FirstFit;}|WF{$$=WorstFit;};

%%
