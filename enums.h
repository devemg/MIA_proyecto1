#ifndef ENUMS_H
#define ENUMS_H

enum Fit {BestFit='b',FirstFit='f',WorstFit='w'};
enum Unit {KB='k',MB='m',Byte='b'};
enum Status {Activo=1,Inactivo=0};
enum TipoParticion {Primaria='p',Extendida='e',Logica='l'};
enum Response {SUCCESS,
               ERROR_INSUFICIENT_SPACE,
               ERROR_FULL_PARTITION_PRIMARY,
               ERROR_UNHANDLED,
               ERROR_EXISTS_EXTEND_PARTITION};

#endif // ENUMS_H
