%option noyywrap
%{
#include "parser.h"
#include <iostream>

int columna=0;
%}
%option caseless

letra [a-zñA-ZÑ]
comentario "/*"[^'*']*"*/"
digito [0-9]
iden {letra}+({letra}|{digito})*
Numero {digito}+
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

"mkdisk"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return MKDISK; }
"rmdisk"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return RMDISK; }
"fdisk"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return FDISK; }
"mount"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return MOUNT; }
"unmount"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return UNMOUNT; }
"rep"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return REP; }
"exec"         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return EXEC; }
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

[=]         { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return IGUAL; }
{iden}          { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return WORD; }
{Numero}        { columna=columna+strlen(yylval.TEXT); strcpy(yylval.TEXT, yytext); return NUMERO; }
{comentario}    { /*Se ignoran*/ }
[[:blank:]]     { /*Se ignoran*/ }
.               {std::cout <<yytext<<"Comando incorrecto"<< std::endl;}
%%