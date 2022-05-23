
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SharedFunctions.c"

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

    //---------------------------------------------------------
    //Obtener la llave de la memoria
    key_t memoryKey = getKey(memoryProcessInt);
    tamannio = getSize();
    int memoryId = createMemory(memoryKey,tamannio);
    memory = getMemory(memoryId);

    //---------------------------------------------------------
    key_t semaphoreKey = getKey(semaphoreInt);
	semaphoreId = createSemaphore(semaphoreKey);

    Operation.sem_num = 1;
    Operation.sem_op = 2;
    Operation.sem_flg = 0;

    return 0;

}