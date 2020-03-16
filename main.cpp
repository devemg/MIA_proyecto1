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
  //createDirectory(true,"vda0","/home");
 /* createDirectory(true,"vda0","/home/emely");
  createDirectory(true,"vda0","/home/emely/fotos");
  createDirectory(true,"vda0","/home/flaca/fotos");
  createDirectory(true,"vda0","/home/roberto/fotos");
  */
  createFile("/file.txt",true,15,"/home/emely/Escritorio/testData/disk1.disk","particion_1");
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
  reportBlocks("/home/emely/Escritorio/testData/disk1.disk","particion_1", "/home/emely/Escritorio/testData/blocks.png");
  reportInodes("/home/emely/Escritorio/testData/disk1.disk","particion_1", "/home/emely/Escritorio/testData/inodos.png");
  //reportTree("/home/emely/Escritorio/testData/tree.png","vda0");
  //cout<<"reporte generado\n";
  //myPause();
  //reportSuperBlock("/home/emely/Escritorio/testData/disk1.disk","particion_1","/home/emely/Escritorio/testData/superblock.png");
  //reportBitmap(0,"/home/emely/Escritorio/testData/disk1.disk","particion_1","/home/emely/Escritorio/testData/bitmapRep1.txt");
  //reportBitmap(1,"/home/emely/Escritorio/testData/disk1.disk","particion_1","/home/emely/Escritorio/testData/bitmapRep2.txt");

  return 0;
}
