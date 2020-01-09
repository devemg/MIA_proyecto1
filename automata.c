#include "automata.h"//;
#include <string.h>
#include <stdlib.h>

void analizarComando(char comando[200]){
    int estado = 0;
    //estado 0 = leyendo comando principal
    //estado 1 = leyendo parametro
    //estado 2 = leyendo valor
    //estado 3 = comentario
    int idComando = -1;
    int idParametro = -1;
    //valores para almacenar
    char aux[200];

    char path[100];
    char fit[2];
    char unit[2];
    char size[10];
    char type[2];
    char name[100];
    char id[100];

    aux[0]='\0';

    path[0]='\0';
    fit[0]='\0';
    unit[0]='\0';
    size[0]='\0';
    type[0]='\0';
    name[0]='\0';
    id[0]='\0';
    int contador = 0;
    while (comando[contador]!='\0') {
        //validando numero en el comando
        if(isLetra(comando[contador])==0 && estado == 0){
            printf("ERROR: El comando no existe");
            break;
        }

        switch (estado) {
        case 0:

            //concatenando al auxiliar
            strncat(aux, &comando[contador],1);

            //EVALUANDO SI SE HA COMPLETADO UN COMANDO PRINCIPAL
            evaluarComandos(aux,&estado,&idComando);
            if(estado!=0){
                printf("Estado:%d IdComando:%d\n",estado,idComando);
                aux[0]='\0';
            }
            break;
         case 1:

            //concatenando al auxiliar
            strncat(aux, &comando[contador],1);

            //EVALUANDO SI SE HA COMPLETADO UN PARAMETRO
            if(evaluarParametros(aux,idComando,&estado,&idParametro)==1)break;
            if(estado!=1){
                printf("Estado:%d IdParametro:%d\n",estado,idParametro);
                aux[0]='\0';
            }
            break;
          case 2:
            if(comando[contador]==' '){
                reporte(idParametro,path,fit,unit,size,type,name,id);
                estado=1;
                path[0]='\0';
                fit[0]='\0';
                unit[0]='\0';
                size[0]='\0';
                type[0]='\0';
                name[0]='\0';
                id[0]='\0';
                break;
            }
            //GUARDANDO VALORES
            if(idParametro == 0){
                strncat(path, &comando[contador],1);
            }
            if(idParametro == 1){
                strncat(fit, &comando[contador],1);
            }
            if(idParametro == 2){
                strncat(unit, &comando[contador],1);
            }
            if(idParametro == 3){
                strncat(size, &comando[contador],1);
            }
            if(idParametro == 4){
                strncat(type, &comando[contador],1);
            }
            //add y delete
            if(idParametro == 7){
                strncat(name, &comando[contador],1);
            }
            if(idParametro == 8){
                strncat(id, &comando[contador],1);
            }
            break;
        }
        contador++;
    }
   /* if(aux[0]!='\0' && aux[0]!=' '){
        printf("%c\n",aux[0]);
        printf("ERROR: Sintaxis incorrecta\n");
    }else{
      */  //ultimo parametro
        reporte(idParametro,path,fit,unit,size,type,name,id);
 //   }
}

void reporte(int idComando,char path[],char fit[],char unit[],char size[],char type[],char name[],char id[]){
    printf("REPORTE****************************************************\n");
    printf("ID parametro: %d\n",idComando);
    printf("path: %s\n",path);
    printf("fit: %s\n",fit);
    printf("unit: %s\n",unit);
    printf("size: %s\n",size);
    printf("type: %s\n",type);
    printf("name: %s\n",name);
    printf("id: %s\n",id);
}

int evaluarComandos(char aux[],int *estado,int *idComando){
    if(strcmp(aux, "mkdisk ") == 0){
       *estado = 1;
       *idComando = 0;
    }
    if(strcmp(aux, "rmdisk ") == 0){
       *estado = 1;
       *idComando = 1;
    }
    if(strcmp(aux, "fdisk ") == 0){
       *estado = 1;
       *idComando = 2;
    }
    if(strcmp(aux, "mount ") == 0){
       *estado = 1;
       *idComando = 3;
    }
    if(strcmp(aux, "unmount ") == 0){
       *estado = 1;
       *idComando = 4;
    }
    if(strcmp(aux, "pause ") == 0){
       *estado = 1;
       *idComando = 5;
    }
    if(strcmp(aux, "rep ") == 0){
       *estado = 1;
       *idComando = 6;
    }
    if(strcmp(aux, "exec ") == 0){
       *estado = 1;
       *idComando = 7;
    }
    return 0;
}

int evaluarParametros(char aux[],int idComando,int *estado,int *idParametro){
   //printf("idComando %d\n\n",idComando);

    if(strcmp(aux, "-path~:~") == 0){
        if(idComando == 4 || idComando == 5){
            printf("ERROR:El parametro %s no corresponde al comando\n",aux);
            return 1;
        }
       *estado = 2;
       *idParametro = 0;
    }
    if(strcmp(aux, "-fit~:~") == 0){
        if(idComando != 0 && idComando != 2){
            printf("ERROR:El parametro %s no corresponde al comando\n",aux);
            return 1;
        }
       *estado = 2;
       *idParametro = 1;
    }
    if(strcmp(aux, "-unit~:~") == 0){
        if(idComando != 0 && idComando != 2){
            printf("ERROR:El parametro %s no corresponde al comando\n",aux);
            return 1;
        }
       *estado = 2;
       *idParametro = 2;
    }
    if(strcmp(aux, "-size~:~") == 0){
        if(idComando != 0 && idComando != 2){
            printf("ERROR:El parametro %s no corresponde al comando\n",aux);
            return 1;
        }
       *estado = 2;
       *idParametro = 3;
    }
    if(strcmp(aux, "-type~:~") == 0){
        if(idComando != 2 ){
            printf("ERROR:El parametro %s no corresponde al comando\n",aux);
            return 1;
        }
       *estado = 2;
       *idParametro = 4;
    }
    if(strcmp(aux, "-delete~:~") == 0){
        if(idComando != 2 ){
            printf("ERROR:El parametro %s no corresponde al comando\n",aux);
            return 1;
        }
       *estado = 2;
       *idParametro = 5;
    }
    if(strcmp(aux, "-add~:~") == 0){
        if(idComando != 2 ){
            printf("ERROR:El parametro %s no corresponde al comando\n",aux);
            return 1;
        }
       *estado = 2;
       *idParametro = 6;
    }
    if(strcmp(aux, "-name~:~") == 0){
        if(idComando != 2 &&idComando != 3 &&idComando != 6 ){
            printf("ERROR:El parametro %s no corresponde al comando\n",aux);
            return 1;
        }
       *estado = 2;
       *idParametro = 7;
    }
    if(strcmp(aux, "-id~:~") == 0){
        if(idComando != 6 &&idComando != 4 ){
            printf("ERROR:El parametro %s no corresponde al comando\n",aux);
            return 1;
        }
       *estado = 2;
       *idParametro = 8;
    }
    return 0;
}

int isLetra(char caracter){
  return (caracter >= 'a' && caracter <= 'z') || (caracter >= 'A' && caracter <= 'Z')||caracter==' ';
}

int isNumero(char caracter){
    return  (caracter >= '0' && caracter <= '9');
}

void limpiar(char cadena[])
{
  char *p;
  p = strchr(cadena, '\n');
  if (p)
    *p = '\0';
}

void toLowerCase(char comando[]){
    int contador=0;
    while (comando[contador]!='\0') {
        if(comando[contador] >= 'A' && comando[contador] <= 'Z'){
            comando[contador] = comando[contador]+32;
        }
        contador++;
    }
}
