
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SharedFunctions.c"

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



int main (){
    /*
    Compilacion
    gcc Finisher.c -o p4  
    Ejecutar
    ./p4 
    */

    //---------------------------------------------------------
    //Obtener la llave de la memoria
    key_t memoryKey = getKey(memoryProcessInt);
    tamannio = getSize();
    int memoryId = createMemory(memoryKey,tamannio);
    memory = getMemory(memoryId);
    //Libera la Memoria compartida.
	shmctl (memoryId, IPC_RMID, (struct shmid_ds *)NULL);

    //---------------------------------------------------------
    key_t semaphoreKey = getKey(semaphoreInt);
	semaphoreId = createSemaphore(semaphoreKey);

    Operation.sem_num = 1;
    Operation.sem_op = 2;
    Operation.sem_flg = 0;
    //Da el paso, lo cual genera que el goAhead en el producer
    //se ponga en cero y termine el programa.
    semop (semaphoreId, &Operation, 1);

    //---------------------------------------------------------
    //Se cierra la bitacora aunque no es necesario porque siempre
    //Se abre y se cierra.
	FILE* fichero; 
	fichero = openFile("binnacle.txt","a+");
	fclose(fichero);

    return 0;

}