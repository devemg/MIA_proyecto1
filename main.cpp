#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <handlerdisk.h>
#include <enums.h>
#include <iostream>
#include <handlerpartitions.h>
using namespace std;
/*
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
}*/

int main()
{
  cout<<"Hola mundo :D\n";
  newDisk(30000,FirstFit,KB,"/home/emely/Escritorio/testData","disk1");
  //deleteDisk("/home/emely/Escritorio/testData/disk1.disk");
  createPartition(300,KB,"/home/emely/Escritorio/testData","disk1",Primaria,FirstFit,"particion 1");
  reportMBR("/home/emely/Escritorio/testData","disk1Rep.jpg");

  return 0;
}
