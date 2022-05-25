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
union semun arg;

int semaphoreIDFiles;
struct sembuf OperationF;
union semun argF;



//Funciones del semaforo para la memoria.
void waitMemory(){
	Operation.sem_op = -1;
	semop (semaphoreId, &Operation, 1);
}

void signalMemory(){
	Operation.sem_op = 1;
	semop (semaphoreId, &Operation, 1);
}

//Funciones del semaforo para los archivos.
void waitFiles(){
	OperationF.sem_op = -1;
	semop (semaphoreIDFiles, &OperationF, 1);
}

void signalFiles(){
	OperationF.sem_op = 1;
	semop (semaphoreIDFiles, &OperationF, 1);
}



void executionProcess(){
    printf("Procesos en ejecucion:\n");

    /*Entra en la memoria compartida y muestra el proceso en ejecucion
    *Como los procesos no salen le la memoria siempre muesta el PID 1 
    *Cuendo se saquen los procesos ya se mostrada el proceso real en ejecucion 
    */ 
    for (int i=0; i<tamannio; i++){
        if (memory[i].state == 1 )
        {
            printf("\tPID: %d en la pagina: %d \n",memory[i].PID, i);
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
        printf("\tPID: %s", contents);
    }

    fclose(input_file);
    free(contents);
}

void blockProcess(){
    printf("Procesos bloqueados:\n");
    readFile("Block_process.txt");
}

void searchProcess(){
    printf("\nProceso en busqueda:");
    printf("\t[Nota]:Se muestran dos PID's (1) ultimo en buscar (2) actual \n");
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
    printf("[Nota]: PID: 0 denota que no hay proceso\n");
    waitMemory();
    executionProcess();
    signalMemory();
    
    waitFiles();
    blockProcess();
    signalFiles();
    
    waitFiles();
    searchProcess();
    signalFiles();

    waitFiles();
    deadProcess();
    signalFiles();
    
    waitFiles();
    finishedProcess();
    signalFiles();
}


void showMemoryState(){
    
    printf("#pagina\tProceso\n");
    waitMemory();
	for (int i=0; i<tamannio; i++){
        printf("   %d\t%d\n", i,memory[i].PID);
	}
    signalMemory();

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

    if (stateType == SHOWPROCESS){ // 1 procesos

        showProcessState();
    
    }else if(stateType == SHOWMEMORY){ // 0 memoria
        
        showMemoryState();
    
    }else{
        printf("ERROR: entrada invalida.\n");
        printf("1 para mostrar estado de procesos\n");
        printf("0 para mostrar estado de memoria\n");
    }

    //---------------------------------------------------------------
    key_t semaphoreKey = getKey(semaphoreInt);
	semaphoreId = createSemaphore(semaphoreKey);
    Operation.sem_num = 0; Operation.sem_flg = 0;

    //---------------------------------------------------------------
    key_t semaphoreKeyF = getKey(semaphoreInt);
	semaphoreIDFiles = createSemaphore(semaphoreKeyF);
    OperationF.sem_num = 0; OperationF.sem_flg = 0;


    return 0;
}
