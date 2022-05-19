#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include "Process.c"
#define IPC_RESULT_ERROR (-1)

//---------------------FUNCIONES COMPARTIDAS DE ARCHIVOS---------------------//
FILE* openFile(const char* name, char *modo){
	/**
	 * @brief Abre un archivo y devuelve el puntero a este
	 * 
	 */
	FILE *file;
	if ((file = fopen(name,modo)) == NULL){
		printf("Error! opening file");
		exit(1);
	}
	return file;
}


int getSize(){
	/**
	 * @brief Devuelve el tamaño de la memoria compartida, 
	 * 
	 */
    FILE *file;
	int num;
	if ((file = fopen("Size.txt","r")) == NULL){
		printf("Error! opening file");
		exit(1);
	}
	fscanf(file,"%d", &num);
	fclose(file);
	return num;
}


void addToBinnacle(int id_Proceso, char* mensaje, int success){
	/**
	 * @brief Escribe en la bitacora PID, acción, tipo, 
	 * hora, espacio asignado, entrò o no a la memoria.
	 */

	FILE* file;     
	file = openFile("bitacora.txt","a+");
	if (success)
	{
    	fprintf (file, "%s", mensaje); //Aqui poner todos los datos del procesp
	}else{
		fprintf (file, "%s", mensaje);
	}
			
    fclose(file);

}



//-----------------------FUNCIONES DE MEMORIA--------------------------//

key_t getKey(int IDMem){ 
    /**
     * @brief Funcion que obtiene la key para la memoria compartida
     *  Se le pasa un fichero existente, y un entero cualquiera que 
     *  es el mem_id, todos los que quieran la key deben usar mismo
     *  fichero y mem_id para usar el mismo espacio en memoria.
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

Process* getMemory(int IdMem){ 
	/**
	 * @brief Obtiene la memoria compartida, dado un id de memoria
	 * 
	 */
	
	Process* memory = (Process *)shmat (IdMem, 0, 0);
	if (memory == NULL)
	{
		printf("There is not any memory available!\n");
		exit(0);
	}
	return memory;
}



