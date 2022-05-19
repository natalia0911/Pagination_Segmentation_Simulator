
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include "AuxiliarFunctions.c"


#define IPC_RESULT_ERROR (-1)



int createMemory(key_t key, int tamannio){
    /**
     * @brief Crea la memoria compartida y el espacio para esta
     * segun la key asociada
     */
	int IdMemoria = shmget (key, sizeof(Process)*tamannio, 0777 | IPC_CREAT);
	if (IdMemoria == IPC_RESULT_ERROR)
	{
		printf("Error trying to get the Id!\n");
		exit(0);
	}
    return IdMemoria;

}

void prepareMemory(int n, Process* Memory){
    //Inicializa cada casilla con un 0 en su 'estado'
	for (int i=0; i<n; i++)
	{
		Memory[i].state = 0;
		printf("Escrito %d\n", Memory[i].state);
	}
}


key_t getKey(int IDMem){ 
    /**
     * @brief Funcion que obtiene la key para la memoria compartida
     *  Se le pasa un fichero existente, y un entero cualquiera que 
     *  es el mem_id, todos los que quieran la key deben usar mismo
     *  fichero y mem_id para usar el mismo espacio en memoria.
     * 
     *  NOTA> tal vez esta funcion se necesite en el productor y 
     *  deberia estar en otro archivo.
     */
	key_t key;

	key = ftok ("/bin/ls", IDMem);
	if (key == IPC_RESULT_ERROR)
	{
		printf("Error trying to get the key!\n");
		exit(0);
	}
	return key;
}



int main(){

    key_t k;
    k = getKey(100);
    int tamannio = 15;
    int IdMemory = createMemory(k,tamannio);
    printf("Numero de memoria xd: %i\n",IdMemory);
    saveSizeinFile(tamannio);
    initializeBinnacle();

    Process *memory = getMemory(IdMemory);

    prepareMemory(tamannio,memory);
    //Liberar memoria.
	shmdt ((char *)memory);

    /* 
    PARA VER SEGMENTOS EN MEMORIA
    >> ipcs -a
    PARA BORAR SEGMENTOS EN MEMORIA, SHMID SEGUN CORRESPONDA
    >> ipcrm -m shmid

    gcc Initializer.c
    ./a.out
    */
    
}