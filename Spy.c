//Librerias 
#include <stdio.h>

//Constantes

#define SHOWPROCESS 1 
#define SHOWMEMORY 0
/*
* @yosuabd
* parametro: 1 para estado de procesos 
*            0 para estado de memoria     
*
* funcionamiento: Mostrar la memoria compartida y los diferentes estados de los procesosd
*/

void showProcessState(){
    
    printf("showProcessState()");

}


void showMemoryState(){
    
    printf("showMemoryState()");

}

int main(int argc, char const *argv[])
{

    /*
    Compilacion
    gcc Spy.c -o p3  
    Parametros, 1 procesos y 0 memoria
    ./p3 [1|0]
    */

    //Valida que entre por parametro el tipo
    if (argc < 1){printf("ERROR: Parametro requerido.\n"); return 0;}
    
    //Selecciona el tipo 
    int stateType = atoi(argv[1]);

    if (stateType == SHOWPROCESS){

        showProcessState();
    
    }else if(stateType == SHOWMEMORY){ // Memory
        
        showMemoryState();
    
    }else{
        printf("ERROR: entrada invalida.\n");
        printf("1 para mostrar estado de procesos\n");
        printf("0 para mostrar estado de memoria\n");
    }

    return 0;
}
