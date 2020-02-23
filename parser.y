%{
#include "scanner.h"
#include <iostream>
#include <string.h>
#include <enums.h>
#include <utils.h>

extern int yylineno; //linea actual
extern int columna; //columna actual
extern char *yytext; //lexema actual

class Option
{
public:
    Option(Options op){
        option = op;
        next = NULL;
    }

    Options option;
    char *text;
    int num;
    Fit fit;
    Unit unit;
    TipoParticion type;
    DeleteType delType;

    Option *next;
};

class Command {
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

int yyerror(const char* mens){
//metodo que se llama al haber un error sintactico
//SE IMPRIME EN CONSOLA EL ERROR
std::cout<<"ERROR EN COMANDO: "<<mens<<" "<<yytext<< std::endl;
return 0;
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

bool validateOptionCommand(CommandEnum cmd,Option *opt){
    char *name;
    char *path;
    char *id;
    int size = -1;
    Option *it;

    switch (cmd) {
    case mkdisk:
        //opciones obligatorias
        it = opt;
        while(it!=NULL){
            switch (it->option) {
            case Size:
                if(it->num > 0){
                    size = it->num;
                }else{
                    std::cout<<"El tamaño debe ser mayor o igual a cero. \n";
                    return false;
                }
                break;
            case Fitt:
            case Unitt:
                break;
            case Path:
                path = it->text;
                break;
            default:
                std::cout<<"Opción \""<<showOption(it->option)<<"\" no válida para el comando.\n";
                break;
            }
         it = it->next;
        }

        if(size == -1){
            std::cout<<"Debe indicar un tamaño en línea "<<yylineno<<"\n";
            return false;
        }
        if(strcmp(path,"\0")==0){
            std::cout<<"Debe indicar un archivo en línea "<<yylineno<<".\n";
            return false;
        }
        break;
    case rmdisk:
        //opciones obligatorias
        //path
         it = opt;
        while(it!=NULL){
            switch (it->option) {
            case Path:
                path = it->text;
                break;
            default:
                std::cout<<"Opción \""<<showOption(it->option)<<"\" no válida para el comando.\n";
                break;
            }
         it = it->next;
        }
        if(strcmp(path,"\0")==0){
            std::cout<<"Debe indicar un archivo en línea "<<yylineno<<".\n";
            return false;
        }
        break;
    case mount:
        //opciones obligatorias
        //path name
        it = opt;
        while(it!=NULL){
            switch (it->option) {
            case Path:
                path = it->text;
                break;
            case Name:
                name = it->text;
                break;
            default:
                std::cout<<"Opción \""<<showOption(it->option)<<"\" no válida para el comando.\n";
                break;
            }
         it = it->next;
        }
        if(strcmp(path,"\0")==0){
            std::cout<<"Debe indicar un archivo en línea "<<yylineno<<".\n";
            return false;
        }
        if(strcmp(name,"\0")==0){
            std::cout<<"Debe indicar un nombre en línea "<<yylineno<<".\n";
            return false;
        }
        break;
    case unmount:
        //opciones obligatorias
        //name
        it = opt;
        while(it!=NULL){
            switch (it->option) {
            case Name:
                name = it->text;
                break;
            default:
                std::cout<<"Opción \""<<showOption(it->option)<<"\" no válida para el comando.\n";
                break;
            }
         it = it->next;
        }
        if(strcmp(name,"\0")==0){
            std::cout<<"Debe indicar un nombre en línea "<<yylineno<<".\n";
            return false;
        }
        break;
   case rep:
        //opciones obligatorias
        //path name id
        it = opt;
        int len;
        int index;
        while(it!=NULL){
            switch (it->option) {
            case Path:
                path = it->text;
                break;
            case Name:
                name = it->text;
                len = strlen(name);
                for (index = 0; index < len; ++index)
                   name[index] = tolower(name[index]);
                if(strcmp(name,"mbr")!=0 && strcmp(name,"disk")!=0){
                    std::cout<<"Debe indicar el tipo de reporte: \"mbr\" o \"disk\" en línea"<<yylineno<<".\n";
                    return false;
                }

                break;
            case Id:
                id = it->text;
                break;
            default:
                std::cout<<"Opción \""<<showOption(it->option)<<"\" no válida para el comando.\n";
                break;
            }
         it = it->next;
        }
        if(strcmp(path,"\0")==0){
            std::cout<<"Debe indicar un archivo en línea "<<yylineno<<".\n";
            return false;
        }
        if(strcmp(name,"\0")==0){
            std::cout<<"Debe indicar un nombre en línea "<<yylineno<<".\n";
            return false;
        }
        if(strcmp(id,"\0")==0){
            std::cout<<"Debe indicar un id en línea "<<yylineno<<".\n";
            return false;
        }
        break;
     case fdisk:
        //opciones obligatorias
        //path name id
        it = opt;
        while(it!=NULL){
            switch (it->option) {
            case Size:
                if(it->num > 0){
                    size = it->num;
                }else{
                    std::cout<<"El tamaño debe ser mayor o igual a cero. \n";
                    return false;
                }
                break;
            case Path:
                path = it->text;
                break;
            case Name:
                if(strcmp(path,"mbr")==0){
                    std::cout<<"Debe indicar un archivo en línea "<<yylineno<<".\n";
                    return false;
                }
                name = it->text;
                break;
            case Id:
                id = it->text;
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
        if(strcmp(path,"\0")==0){
            std::cout<<"Debe indicar un archivo en línea "<<yylineno<<".\n";
            return false;
        }
        if(strcmp(name,"\0")==0){
            std::cout<<"Debe indicar un nombre en línea "<<yylineno<<".\n";
            return false;
        }
        if(it->option != Delete)if(strcmp(id,"\0")==0){
            std::cout<<"Debe indicar un id en línea "<<yylineno<<".\n";
            return false;
        }
        if(it->option != Delete)if(size == -1){
            std::cout<<"Debe indicar un tamaño en línea "<<yylineno<<"\n";
            return false;
        }
        break;
    }
    return true;
}

%}

%error-verbose

%union{
#include <enums.h>

//se especifican los tipo de valores para los no terminales y lo terminales
char TEXT [256];
Fit FIT;
TipoParticion TIPOPARTICION;
DeleteType DELETETYPE;
Unit UNIT;
CommandEnum COMMAND_ENUM;
struct Option * OPTION;
class Command *COMMAND;

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
%type<TEXT>  INICIO
%type<FIT>  FIT_OPTIONS
%type<TIPOPARTICION>  TYPE_OPTIONS
%type<DELETETYPE>  DELETE_OPTIONS
%type<UNIT>  UNIT_OPTIONS

%type<OPTION>  OPTIONS_LIST
%type<OPTION>  STATE_OPTION

%type<COMMAND_ENUM>  STATE_COMMANDS
%type<COMMAND>  COMMANDS_LIST
%type<COMMAND>  COMMAND


%start INICIO

%%

INICIO :COMMANDS_LIST{
   /* Command *first = $1;
    while(first!=NULL){
    std::cout<<"COMANDO: "<<first->cmd<<std::endl;
    first = first->next;
    }*/
};

COMMANDS_LIST: COMMANDS_LIST COMMAND{
  /*  if($1!=NULL){
        if($2!=NULL){
            Command *first = $1;
            while(first->next!=NULL){
                first = first->next;
            }
            first->next = $2;
        }
    }
    $$=$1;*/
}
|COMMAND{
  //  $$ = $1;
};

COMMAND: STATE_COMMANDS OPTIONS_LIST{

    if(validateOptionCommand($1,$2)){
      //  $$ = new Command($1,$2);
    }
};

STATE_COMMANDS: MKDISK{$$=mkdisk;}|RMDISK{$$=rmdisk;}|FDISK{$$=fdisk;}|MOUNT{$$=mount;}|UNMOUNT{$$=unmount;}|REP{$$=rep;}
|EXEC{$$=exec;};


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
$$->text = $3;
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
|PATH IGUAL WORD{
$$ = new Option(Path);
$$->text = $3;
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
 $$->text = $3;
};

DELETE_OPTIONS:FAST{$$=Fast;}|FULL{$$=Full;};

TYPE_OPTIONS:P_{$$=Primaria;}|E_{$$=Extendida;}|L_{$$=Logica;};

UNIT_OPTIONS:K_B{$$=KB;}|M_B{$$=MB;}|B_{$$=Byte;};

FIT_OPTIONS:BF{$$=BestFit;}|FF{$$=FirstFit;}|WF{$$=WorstFit;};

%%
