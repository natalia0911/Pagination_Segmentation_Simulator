//Librerias 
#include <stdio.h>
#include "SharedFunctions.c"
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

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

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
// union ya definida en sys/sem.h
#else
union semun { 
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
	struct seminfo *__buf;
};
#endif

//Variables para los semaforos
int semaphoreId;
struct sembuf Operation;
struct sembuf OFinalice;
union semun arg;
int goAhead = 1;

//Funciones del semaforo
void wait(){
    //printf("En wait...\n");
	Operation.sem_op = -1;
	semop (semaphoreId, &Operation, 1);
}

void signal(){
    //printf("Signal...\n");
	Operation.sem_op = 1;
	semop (semaphoreId, &Operation, 1);
}


void executionProcess(){
    printf("Proceso en ejecucion:\n");

    /*Entra en la memoria compartida y muestra el proceso en ejecucion
    *Como los procesos no salen le la memoria siempre muesta el PID 1 
    *Cuendo se saquen los procesos ya se mostrada el proceso real en ejecucion 
    */ 
    for (int i=0; i<tamannio; i++){
        if (memory[i].state == 1 )
        {
            printf("PID: %d en la pagina: %d \n",memory[i].PID, i);
        }
	}
}

void readFile(const char* filename){
    FILE* input_file = fopen(filename, "r");
    if (!input_file)
        exit(EXIT_FAILURE);

    char *contents = NULL;
    size_t len = 0;
    while (getline(&contents, &len, input_file) != -1){
        printf("PID: %s", contents);
    }

    fclose(input_file);
    free(contents);
}

void blockProcess(){
    printf("Procesos bloqueados:\n");
    readFile("Block_process.txt");
}

void searchProcess(){
    printf("\nProceso en busqueda:\n");
    readFile("Search_process.txt");
}

void deadProcess(){
    printf("\nProcesos muertos:\n");
    readFile("Dead_process.txt");
}

void finishedProcess(){
    printf("\nProcesos terminados:\n");
    readFile("Finished_process.txt");
}

void showProcessState(){
    
    executionProcess();
    blockProcess();
    searchProcess();
    deadProcess();
    finishedProcess();
}


void showMemoryState(){
    
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

    //-------------------------------------------------------------------------------------------------------------------------
    key_t semaphoreKey = getKey(semaphoreInt);
	semaphoreId = createSemaphore(semaphoreKey);
	
	arg.val = 1;
	semctl (semaphoreId, 0, SETVAL, 1); // 0 es e indice del semaforo, el 1 es semaforo disponible

	Operation.sem_num = 0;
	Operation.sem_op = 1;
	Operation.sem_flg = 0;

	OFinalice.sem_num = 1;
	OFinalice.sem_op = -2;	
	OFinalice.sem_flg = 0;

    return 0;
}
