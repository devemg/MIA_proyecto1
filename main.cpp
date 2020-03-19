#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <handlerdisk.h>
#include <enums.h>
#include <iostream>
#include <handlerpartitions.h>
#include <parser.h>
#include <scanner.h>
#include <handlerfilesystem.h>
#include<handler_reports.h>

using namespace std;

int main()
{
    /*
    std::string command;
    while(true){
        cout << ">";
        getline (std::cin,command);
        if(strcmp(command.c_str(),"exit")==0){
            break;
        }else if(strcmp(command.c_str(),"cls")==0){
            system("clear");
            continue;
        }
        writeCommand(command,false);
        readExecCommand(false);
    }
*/

  newDisk(50,FirstFit,MB,"/home/emely/Escritorio/testData","disk1");
  createPartition(30,MB,"/home/emely/Escritorio/testData","disk1",Primaria,WorstFit,"particion_1");
  formatPart("/home/emely/Escritorio/testData/disk1.disk","particion_1",Fast,ext2);
  mountPart("/home/emely/Escritorio/testData/disk1.disk","particion_1");

  createDirectory(true,"vda0","/a");

  int i;
  //llenando apuntadores directos
  for(i = 0;i<23;i++){
      createDirectory(true,"vda0","/a");
  }


  //llenando indirecto apuntador simple
  for(i = 0;i<15;i++){
      createDirectory(true,"vda0","/a");
  }
/*
  //llenando indirecto apuntador doble 225
  for(i = 0;i<50;i++){
      createDirectory(true,"vda0","/a");
  }
*/

  /*
  //llenando indirecto apuntador triple
    for(i = 0;i<55;i++){
      createDirectory(true,"vda0","/a");
  }
  */

  //createDirectory(true,"vda0","/home/emely");
  //createDirectory(true,"vda0","/home/emely/fotos");
  //createDirectory(true,"vda0","/home/flaca/fotos");
  //createDirectory(true,"vda0","/home/roberto/fotos");

  //createFile("/home/file1.txt",true,10,"/home/emely/Escritorio/testData/disk1.disk","particion_1");
  //createFile("/home/fil2.txt",true,100,"/home/emely/Escritorio/testData/disk1.disk","particion_1");

  //createFile("/home/fil2.txt",true,100,"/home/emely/Escritorio/testData/disk1.disk","particion_1");

  //createFile("/home/file.txt",true,"/home/emely/Escritorio/texto.txt","/home/emely/Escritorio/testData/disk1.disk","particion_1");
  //catFile("/home/file.txt","/home/emely/Escritorio/testData/disk1.disk","particion_1");

/*
   reportFile("/home/file.txt","/home/emely/Escritorio/testData/disk1.disk",
             "particion_1",
             "/home/emely/Escritorio/testData/file.png");
*/
  /*
  createDirectory(true,"vda0","/bin");
  createDirectory(true,"vda0","/usr");
  createDirectory(true,"vda0","/a");
  createDirectory(true,"vda0","/b");
  createDirectory(true,"vda0","/c");
  createDirectory(true,"vda0","/d");
  createDirectory(true,"vda0","/e");
  */

  //showMounts();
  //reportBlocks("/home/emely/Escritorio/testData/disk1.disk","particion_1", "/home/emely/Escritorio/testData/blocks.png");
  //reportInodes("/home/emely/Escritorio/testData/disk1.disk","particion_1", "/home/emely/Escritorio/testData/inodos.png");
  reportTree("/home/emely/Escritorio/testData/tree.png","vda0");
  //cout<<"reporte generado\n";
  //myPause();
  //reportSuperBlock("/home/emely/Escritorio/testData/disk1.disk","particion_1","/home/emely/Escritorio/testData/superblock.png");
  //reportBitmap(0,"/home/emely/Escritorio/testData/disk1.disk","particion_1","/home/emely/Escritorio/testData/bitmap_inodos.txt");
  //reportBitmap(1,"/home/emely/Escritorio/testData/disk1.disk","particion_1","/home/emely/Escritorio/testData/bitmap_bloques.txt");

  return 0;
}
