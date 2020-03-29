%option noyywrap
%{
#include "parser.h"
#include <iostream>

int columna=0;
%}
%option caseless

letra [a-zñA-ZÑ]
comentario #[^'\n']*\n
digito [0-9]
iden {letra}+({letra}|{digito})*
iden2 \"{letra}+({letra}|{digito}|[ ])*\"
Numero ("-")?{digito}+
path (\/[a-z]+[a-z0-9_\-\.]+)+
path2 \"(\/[a-z]+[a-z0-9_\-\. ]+)+\"

%%
"-size"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return SIZE; }
"-fit"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return FIT; }
"-unit"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return UNIT; }
"-path"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return PATH; }
"-type"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return TYPE; }
"-delete"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return DELETE; }
"-name"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return NAME; }
"-add"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return ADD; }
"-id"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return ID; }
"-fs"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return FS; }
"-usr"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return USR; }
"-pwd"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return PWD; }
"-grp"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return GRP; }
"-ugo"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return UGO; }
"-r"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return RECURSIVE; }
"-p"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return RECURSIVE; }
"-cont"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return CONTENT; }
"-file"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return _FILE; }
"-dest"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return DESTINY; }
"-ruta"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return RUTA; }

"mkdisk"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return MKDISK; }
"rmdisk"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return RMDISK; }
"fdisk"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return FDISK; }
"mount"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return MOUNT; }
"unmount"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return UNMOUNT; }
"rep"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return REP; }
"exec"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return EXEC; }

"recovery"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return RECOVERY; }
"loss"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return LOSS; }
"chgrp"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return CHGRP; }
"chown"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return CHOWN; }
"find"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return FIND; }
"mv"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return MV; }
"cp"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return CP; }
"mkdir"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return MKDIR; }
"ren"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return REN; }

"edit"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return EDIT; }
"rem"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return REM; }
"cat"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return CAT; }
"mkfile"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return MKFILE; }
"chmod"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return CHMOD; }

"rmusr"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return RMUSR; }
"mkusr"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return MKUSR; }
"mkgrp"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return MKGRP; }
"rmgrp"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return RMGRP; }
"login"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return LOGIN; }
"mkfs"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return MKFS; }

"logout"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return LOGOUT; }
"pause"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return PAUSE; }

"BF"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return BF; }
"FF"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return FF; }
"WF"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return WF; }
"B"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return B_; }
"K"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return K_B; }
"M"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return M_B; }
"P"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return P_; }
"E"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return E_; }
"L"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return L_; }
"fast"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return FAST; }
"full"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return FULL; }

"2fs"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return EXT2; }
"3fs"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return EXT3; }

[=]         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return IGUAL; }
{path}      { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return PATH_; }
{path2}     {
                std::string str(yytext);
                str = str.substr(1,strlen(yytext)-2);
                columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, &str[0]); return PATH_;
            }
{iden}          { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return WORD; }
{iden2}     {
                std::string str(yytext);
                str = str.substr(1,strlen(yytext)-2);
                columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, &str[0]); return WORD;
            }
{Numero}        { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return NUMERO; }
{comentario}    {/* std::cout <<yytext<<std::endl; */}
[[:blank:]]     { /*Se ignoran*/ }
.               {std::cout <<yytext<<"Comando incorrecto"<< std::endl;}
%%
