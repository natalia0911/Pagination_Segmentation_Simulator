#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

#define IPC_RESULT_ERROR (-1)


int createMemoryCasilla(key_t key, int tamano){
    /**
     * @brief Crea la memoria compartida y el espacio para esta
     * segun la key asociada
     */
	int Id_Memoria = shmget (key, tamano, 0777 | IPC_CREAT);
	if (Id_Memoria == IPC_RESULT_ERROR)
	{
		printf("Error trying to get the Id!\n");
		exit(0);
	}
    return Id_Memoria;

}


key_t getKey(int mem_id){ 
    /**
     * @brief Funcion que obtiene la key para la memoria compartida
     * 
     */
	key_t clave;

	clave = ftok ("/bin/ls", mem_id);
	if (clave == IPC_RESULT_ERROR)
	{
		printf("Error trying to get the key!\n");
		exit(0);
	}
	return clave;
}



int main(){

    key_t k;
    k = getKey(1);
    int entero = createMemoryCasilla(k,1024);
    printf("Numero de memoria xd: %i\n",entero);

}