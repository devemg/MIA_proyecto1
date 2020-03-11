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
  //createDir(true,"vda0","/root/emely");
  showMounts();
  reportTree("/home/emely/Escritorio/testData/tree.png","vda0");
  cout<<"reporte generado\n";
  //myPause();
  //reportSuperBlock("/home/emely/Escritorio/testData/disk1.disk","particion_1","/home/emely/Escritorio/testData/superblock.png");
  //cout<<"REPORTE CREADO\n";
  /*SuperBlock *sb = readSuperBlock("/home/emely/Escritorio/testData/disk1.disk","particion_1");
  if(sb!=NULL){
      cout<<"CANTIDAD MONTADA: "<<sb->s_mnt_count<<endl;
  }

  mountPart("/home/emely/Escritorio/testData/disk1.disk","particion_1");

  sb = readSuperBlock("/home/emely/Escritorio/testData/disk1.disk","particion_1");
    if(sb!=NULL){
        cout<<"CANTIDAD MONTADA: "<<sb->s_mnt_count<<endl;
    }
*/
  //reportBitmap(0,"/home/emely/Escritorio/testData/disk1.disk","particion_1","/home/emely/Escritorio/testData/bitmapRep1.txt");
  //reportBitmap(1,"/home/emely/Escritorio/testData/disk1.disk","particion_1","/home/emely/Escritorio/testData/bitmapRep2.txt");

  return 0;
}
