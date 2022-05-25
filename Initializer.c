#include "SharedFunctions.c"


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
	fprintf (file, "%s","PID\t\tAction\t\t\t\t\tType\t\t\t\tTime\t\t\t\t\t\tAllocated space\t\t\tEntered");
    fclose(file);
}

void prepareMemory(int n, Process* Memory){
    //Inicializa cada casilla con un 0 en su 'estado'
    //No se si sea necesario, pero era para probar acceso a memoria
	for (int i=0; i<n; i++)
	{
		Memory[i].state = 0;  //PONER UN 0
		printf("Escrito %d\n", Memory[i].state);
	}
}


int main(){
    /* 
    PARA VER SEGMENTOS EN MEMORIA
    >> ipcs -a
    PARA BORAR SEGMENTOS EN MEMORIA, SHMID SEGUN CORRESPONDA
    >> ipcrm -m shmid

    PARA COMPILAR
    gcc Initializer.c -o p1
    PARA EJECUTAR
    ./p1
    */


    //Obtener la llave asociada al numero
    key_t memoryKey = getKey(memoryProcessInt);
    //Valor deafult del tamannio, Just in case
    int tamannio = 20;
    //Pedir cantidad de espacios
    printf("Digite la cantidad de espacios de memoria: ");
    scanf("%d",&tamannio);
    //Obtener el id de la memoria segun clave
    int memoryId = createMemory(memoryKey,tamannio);
    printf("Id de memoria: %i\n",memoryId);
    //Guardar el tamaño
    saveSizeinFile(tamannio);
    //Encabezado de la bitacora
    initializeBinnacle();
    //Obtener la memoria con shmat
    Process *memory = getMemory(memoryId);

    //No se si sea necesario, pero era para probar acceso a memoria
    prepareMemory(tamannio,memory);
    printf("Size of process %lu \n", sizeof(Process));
    //separa la memoria compartida en la direccion especificada 
	shmdt ((char *)memory);


    
}