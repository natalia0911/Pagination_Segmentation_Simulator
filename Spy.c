//Librerias 
#include <stdio.h>
#include "SharedFunctions.c"
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
//Proceso como referencia a la memoria compartida
Process *memory;
//Tamaño de la memoria
int tamannio;
void showProcessState(){
    
    printf("showProcessState()");

}


void showMemoryState(){
    
    //printf("showMemoryState");
    printf("#pagina\tProceso\n");

	for (int i=0; i<tamannio; i++){
        printf("   %d\t%d\n", i,memory[i].PID);
	}
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
    
    //Obtener la llave de la memoria
    key_t memoryKey = getKey(100);
    tamannio = getSize();
    //Obtener el id de la memoria segun clave
    int memoryId = createMemory(memoryKey,tamannio);
    printf("Id de memoria: %i\n",memoryId);
    //Obtener la memoria con shmat
    memory = getMemory(memoryId);

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
