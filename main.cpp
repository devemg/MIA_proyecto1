#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <handlerdisk.h>
#include <enums.h>
#include <iostream>
#include <handlerpartitions.h>
#include <parser.h>
#include <scanner.h>

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
   /* const char* x = "temp.txt";
        FILE* input = fopen(x, "r" );
        yyrestart(input);//SE PASA LA CADENA DE ENTRADA A FLEX
        yyparse();//SE INICIA LA COMPILACION
        */



  newDisk(3000,FirstFit,KB,"/home/emely/Escritorio/testData","disk1");
  //deleteDisk("/home/emely/Escritorio/testData/disk1.disk");
  createPartition(1500,KB,"/home/emely/Escritorio/testData","disk1",Primaria,WorstFit,"particion 1.2");
  createPartition(500,KB,"/home/emely/Escritorio/testData","disk1",Extendida,BestFit,"particion 1");
  mountPart("/home/emely/Escritorio/testData/disk1.disk","particion 1.2");
  mountPart("/home/emely/Escritorio/testData/disk1.disk","particion 1");
  showMounts();
/*
  createPartition(250,KB,"/home/emely/Escritorio/testData","disk1",Logica,FirstFit,"particion 2.3");
  createPartition(250,KB,"/home/emely/Escritorio/testData","disk1",Logica,FirstFit,"particion 2.1");
  //deletePartition("/home/emely/Escritorio/testData","disk1","particion 1",Primaria,Full);
  //deletePartition("/home/emely/Escritorio/testData","disk1","particion 1.2",Primaria,Full);
  //deletePartition("/home/emely/Escritorio/testData","disk1","particion 2",Logica,Full);

  reportMBR("/home/emely/Escritorio/testData/disk1.disk","/home/emely/Escritorio/testData/disk1Rep.png");
  reportDisk("/home/emely/Escritorio/testData/disk1.disk","/home/emely/Escritorio/testData/disk1Rep2.png");
  //reportMBR("/home/emely/Escritorio/testData/disk1_mirror.disk","/home/emely/Escritorio/testData/disk1Rep_mirror.png");
*/


  return 0;
}
