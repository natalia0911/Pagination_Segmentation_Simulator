#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

void saveSizeinFile(int size) { 
    /**
     * @brief Guarda en un archivo el tamaño de la memoria compartida
     * 
     */
    FILE *file;
	file = fopen("Size.txt","w");

   	if(file == NULL)
   	{
    	printf("Error!");   
    	exit(1);             
	}

	fprintf(file,"%d",size);
	fclose(file);
}

void initializeBinnacle(){
	/**
	 * @brief Acciones de cada proceso. PID, acción, tipo (asignación, desasignación), 
	 * hora, espacio asignado, entrò o no a la memoria.
	 */
	FILE *file;     
	file = openFile("binnacle.txt","w");
	fprintf (file, "%s","PID\t\tAction\t\t\tType\t\t\tAccion\t\t\t\tTime\t\t\t\tAllocated space\t\t\t\tEntered");
    fclose(file);
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