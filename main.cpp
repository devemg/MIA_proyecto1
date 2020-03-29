#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <handler_disk.h>
#include <enums.h>
#include <iostream>
#include <handler_partitions.h>
#include <parser.h>
#include <scanner.h>
#include <handler_fs.h>
#include<handler_reports.h>
#include <handler_commands.h>

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
        }else if(strcmp(command.c_str(),"showm")==0){
            showMounts();
            continue;
        }else if(strcmp(command.c_str(),"clearm")==0){
            int i;
            for(i=0;i<27;i++){
                partsMounted[i] = NULL;
            }
            continue;
        }else if(strcmp(command.c_str(),"pause")==0){
            system("read -r -p \"Presiona cualquier tecla para continuar...\" key");
            continue;
        }else if(strcmp(command.c_str(),"logout")==0){
            active_sesion->clear();
            continue;
        }
        writeCommand(command,false);
        readExecCommand(false);
    }

/*
  newDisk(50,FirstFit,MB,"/home/emely/Escritorio/testData","disk1");
  createPartition(30,MB,"/home/emely/Escritorio/testData","disk1",Primaria,WorstFit,"particion_1");
  formatPart("/home/emely/Escritorio/testData/disk1.disk","particion_1",Fast,ext2);
  mountPart("/home/emely/Escritorio/testData/disk1.disk","particion_1");
*/
  //addUser("/home/emely/Escritorio/testData/disk1.disk","particion_1","emely","abcd","root");
  //addUser("/home/emely/Escritorio/testData/disk1.disk","particion_1","emely2","abcd","root");
  //reportTree("/home/emely/Escritorio/testData/tree1.png","vda0");

  //catFile("/users.txt","/home/emely/Escritorio/testData/disk1.disk","particion_1");

  //deleteUser("/home/emely/Escritorio/testData/disk1.disk","particion_1","emely");

  //addGroup("/home/emely/Escritorio/testData/disk1.disk","particion_1","users");
  //deleteGroup("/home/emely/Escritorio/testData/disk1.disk","particion_1","users");


  //addUser("/home/emely/Escritorio/testData/disk1.disk","particion_1","emely","abcd","root");

  //addGroup("/home/emely/Escritorio/testData/disk1.disk","particion_1","users");

  //createFile("/file.txt",true,15,"/home/emely/Escritorio/testData/disk1.disk","particion_1");
    //editFile("/file.txt","hola","/home/emely/Escritorio/testData/disk1.disk","particion_1");

  //createFile("/file.txt",true,"/home/emely/Escritorio/texto.txt","/home/emely/Escritorio/testData/disk1.disk","particion_1");
  //catFile("/file.txt","/home/emely/Escritorio/testData/disk1.disk","particion_1");

  //reportTree("/home/emely/Escritorio/testData/tree.png","vda0");


  //2751
  //16151
  //createFile("/coke/file.txt",true,15,"/home/emely/Escritorio/testData/disk1.disk","particion_1");

  //reportFile("/users.txt","/home/emely/Escritorio/testData/disk1.disk","particion_1","/home/emely/Escritorio/testData/file.png");
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

  //showMounts();
  //reportBlocks("/home/emely/Escritorio/testData/disk1.disk","particion_1", "/home/emely/Escritorio/testData/blocks.png");
  //reportInodes("/home/emely/Escritorio/testData/disk1.disk","particion_1", "/home/emely/Escritorio/testData/inodos.png");
  //cout<<"reporte generado\n";
  //myPause();
  //reportSuperBlock("/home/emely/Escritorio/testData/disk1.disk","particion_1","/home/emely/Escritorio/testData/superblock.png");
  //reportBitmap(0,"/home/emely/Escritorio/testData/disk1.disk","particion_1","/home/emely/Escritorio/testData/bitmap_inodos.txt");
  //reportBitmap(1,"/home/emely/Escritorio/testData/disk1.disk","particion_1","/home/emely/Escritorio/testData/bitmap_bloques.txt");

  return 0;
}
