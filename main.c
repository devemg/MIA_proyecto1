#include <stdio.h>
#include <string.h>
#include "automata.h"//;

void leerComando(void){
 char comando[200];
 while (1) {
    printf(">");
    fgets (comando, 200, stdin);
    limpiar(comando);
    if(strcmp("exit",comando)==0)break;
    toLowerCase(comando);
    printf("%s\n",comando);
    analizarComando(comando);
    printf("\n");
 }
}

int main()
{
  leerComando();
    return 0;
}
