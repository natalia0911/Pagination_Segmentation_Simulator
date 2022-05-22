#include <stdio.h>
#include <stdlib.h>


typedef struct Segments{
    //Cantidad de segmentos: de 1 a 5
    short cantidad;
    /*
    Como se sabe que siempre van a ser de 1 a 5 segmentos para no hacerlo más complicado, 
    la estructura de segmentos contiene la cantidad de espacios para cada uno de los segmentos y
    así no hay que recorrer una lista.
    */
    short espaciosSeg1;
    short espaciosSeg2;
    short espaciosSeg3;
    short espaciosSeg4;
    short espaciosSeg5;
}Segments;


typedef struct Process{
    short PID;
    //Para el tiempo
    short burst;
    //Se utiliza si se usa paginación.
    short cantPaginas;
    //0.Espacio vacio, 1.En memoria, 2.Ejecutando
    short state;
    //Se utiliza si se usa Segmentación.
    Segments *seg;

}Process;


Segments *createSegment(short cantidad){
    Segments *seg = malloc(sizeof(Segments));
    seg->cantidad = cantidad;//Cantidad de segmentos.
    seg->espaciosSeg1=0;
    seg->espaciosSeg2=0;
    seg->espaciosSeg3=0;
    seg->espaciosSeg4=0;
    seg->espaciosSeg5=0;
    return seg;
}


Process *createProcessSeg(short PID, short burst, short cantSegmentos){
    Process *p = malloc(sizeof(Process));

    Segments *seg = createSegment(cantSegmentos);
    p->PID = PID;
    p->burst = burst;
    p->seg = seg;
    return p;
}

Process *createProcessPag(short PID, short burst, short cantPaginas){
    Process *p = malloc(sizeof(Process));
    p->PID = PID;
    p->burst = burst;
    p->cantPaginas = cantPaginas;
    return p;
}