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

int main()
{
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

  //newDisk(100,FirstFit,KB,"/home/emely/Escritorio/testData","disk1");
  //deleteDisk("/home/emely/Escritorio/testData/disk1.disk");
  //createPartition(1500,KB,"/home/emely/Escritorio/testData","disk1",Primaria,WorstFit,"particion 1.2");
  //createPartition(500,KB,"/home/emely/Escritorio/testData","disk1",Extendida,BestFit,"particion 1");
  //mountPart("/home/emely/Escritorio/testData/disk1.disk","particion 1.2");

  //mountPart("/home/emely/Escritorio/testData/disk1.disk","particion 1");
  //showMounts();
  //unmountPart("vda1");
  //showMounts();
/*
  createPartition(250,KB,"/home/emely/Escritorio/testData","disk1",Logica,FirstFit,"particion 2.3");
  createPartition(250,KB,"/home/emely/Escritorio/testData","disk1",Logica,FirstFit,"particion 2.1");
  //deletePartition("/home/emely/Escritorio/testData","disk1","particion 1",Primaria,Full);
  //deletePartition("/home/emely/Escritorio/testData","disk1","particion 1.2",Primaria,Full);
  //deletePartition("/home/emely/Escritorio/testData","disk1","particion 2",Logica,Full);

*/
    //reportDisk("/home/emely/Escritorio/testData/disk1.disk","/home/emely/Escritorio/testData/disk1Rep.png");
    //reportMBR("/home/emely/Escritorio/testData/disk1.disk","/home/emely/Escritorio/testData/disk1Rep2.png");

  return 0;
}
