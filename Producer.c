//Librerías  a utilizar
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "SharedFunctions.c"
#include "Queue.c"

//Constantes a utilizar
#define MAXPAGES 10
#define MAXSEGMENTS 5
#define MAXESPACIOS 3
#define MINPAGSEGESP 1

#define MAXBURST 60
#define MINBURST 20

#define MAXTIME 6
#define MINTIME 3

#define MICROSECONDOS 1000000

short PID_AutoIncrement=1;

//Flag que me indica que tipo de algoritmo estoy usando.
int isSEGMENTATION = -1;

//Flag que me indica que tipo de algoritmo estoy usando.
int isRUNNING = 1;

//Proceso como referencia a la memoria compartida
Process *memory;
//Tamaño de la memoria
int tamannio;
//Colas para que luego el espia acceda
Queue *ready;
Queue *finished;
Queue *dead;

Queue *enMemory;


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

void * checkStop(){
    /**
     * @brief Si el finalizador usa el semaforo para finalizar, 
     * se dejan de crear procesos.
     */
	semop (semaphoreId, &OFinalice, 1);
	goAhead = 0;
	exit(0);

}



void saveBlockProcess(){
    /**
     * @yosuabd Funcion que toma el contenido del ready y lo  
     * guarda en un archivo para el espia
     */
    wait();
    if (ready->length>0)
    {
        cleanFile("Block_process.txt");
        Node *temp = ready->first;
        
        while(temp!=NULL){
            addToBlock(temp->process->PID);
            temp = temp->next;
        }
    }
    else{
        cleanFile("Block_process.txt");
        addToBlock(0);
    }
    signal();
    
}

void printProcess(){
	for (int i=0; i<tamannio; i++)
	{
		printf("Espacio: %d\tPID: %d\tEstado %d\n",i,memory[i].PID,memory[i].state);
	}
}

void sacarProceso(int PID){
    /**
     * @brief Liberar un espacio de memoria, donde se encuentre el proceso indicado
     * 
     */
    wait();

    //Se escribe el proceso que se quiere sacar
    //en el archivo para verlo en el espia
    addToFinished(PID);
    for (int i=0; i<tamannio; i++){
        if (memory[i].state == 1 && memory[i].PID==PID){
            memory[i].state = 0;
            memory[i].PID = 0;
            memory[i].burst = 0;
            memory[i].cantPaginas=0;
            memory[i].seg = NULL;
        }
	}
    signal();
}

void *generarDatos(){
    /**
     * @brief Genera procesos de manera random, con las condiciones especificadas de 
     * tiempo y cantidad de paginas o segmentos segun corresponda.
     */
    short burst;
    short espera;
    short PID;
    short cantidadPagSeg;
    short espaciosSeg;

    Process *process; 

    //Usar un flag para detenerlo.
    while(isRUNNING){//De momento usa esa condición para hacer las pruebas
        
        burst = random() % ((MAXBURST) - (MINBURST)+1) + (MINBURST);

        if (isSEGMENTATION==0){//Paginación
            //Ovtiene la cantidad de paginación.
            cantidadPagSeg = random() % ((MAXPAGES) - (MINPAGSEGESP)+1) + (MINPAGSEGESP);
            process = createProcessPag(PID_AutoIncrement++, burst, cantidadPagSeg);

        }else if (isSEGMENTATION==1){//Segmentación.
            //Obtiene la cantidad de segmentos
            cantidadPagSeg = random() % ((MAXSEGMENTS) - (MINPAGSEGESP)+1) + (MINPAGSEGESP);
            process = createProcessSeg(PID_AutoIncrement++, burst, cantidadPagSeg);

            for (int i=0; i<cantidadPagSeg; i++){
                espaciosSeg = random() % ((MAXESPACIOS) - (MINPAGSEGESP)+1) + (MINPAGSEGESP);

                if (i==0) {process->seg->espaciosSeg1=espaciosSeg; continue;}
                if (i==1) {process->seg->espaciosSeg2=espaciosSeg; continue;}
                if (i==2) {process->seg->espaciosSeg3=espaciosSeg; continue;}
                if (i==3) {process->seg->espaciosSeg4=espaciosSeg; continue;}   
                if (i==4) {process->seg->espaciosSeg5=espaciosSeg; continue;}
            }
        }

        //AGREGA EL PROCESO
        enqueue (process, ready);

        
        saveBlockProcess();

        //printQueue(ready, isSEGMENTATION);

        //ESPERA ENTRE minEspera A maxEspera
        espera = random() % ((MAXTIME) - (MINTIME)+1) + (MINTIME);
        printf("STATUS: Esperando %d segundos.\n", espera);
        usleep (espera*MICROSECONDOS);//Usa la variable para pausar el hilo.
            
    }
}


void vaciarMemoria(){
    /**
     * @brief Funcion para ver inicializar o vaciar TODA la memoria compartida (Uso del programador)
     */
    wait();
	for (int i=0; i<tamannio; i++){
		memory[i].state = 0;
        memory[i].PID = 0;
        memory[i].burst = 0;
        memory[i].cantPaginas=0;
        memory[i].seg = NULL;
	}
    signal();
}


void *firstFitSegmentacion(){
    /**
     * @brief Algoritmo de asignacion de memoria para procesos segmentados
     * 
     */

    wait();//semaforo para sincronizar la salida de los procesos.
    Node *nodo = dequeue(ready);
    signal();

    saveBlockProcess();
    if(nodo==NULL) return NULL;
    Process *process = nodo->process;

    short PID = process->PID;
    short burst = process->burst;
    short cantSegmentos = process->seg->cantidad;

    //Inserta el PID actual que este buscando
    // y el ultimo que se puso a buscar o 0 si no hay ultimo. 
    //El 0 es para denotar que no hay nadie buscando
    wait();
    addToSearch(process->PID, 0);
    signal();

    printf("Cantidad de segs %i del PID %d con burst %d\n",cantSegmentos,PID,burst);

    printf("EspSeg1: %i \t",process->seg->espaciosSeg1);
    printf("EspSeg2: %i \t",process->seg->espaciosSeg2);
    printf("EspSeg3: %i \t",process->seg->espaciosSeg3);
    printf("EspSeg4: %i \t",process->seg->espaciosSeg4);
    printf("EspSeg5: %i \n",process->seg->espaciosSeg5);

    int cantidad;//Cantidad de estacios para cada segmento.
    //Para cada segmento.
    int i=0;
    wait();
    for (i;i<cantSegmentos;i++){
        //printf("Recorre los segmentos\n");
        if (i==0) {cantidad=process->seg->espaciosSeg1;}
        if (i==1) {cantidad=process->seg->espaciosSeg2;}
        if (i==2) {cantidad=process->seg->espaciosSeg3;}
        if (i==3) {cantidad=process->seg->espaciosSeg4;}
        if (i==4) {cantidad=process->seg->espaciosSeg5;}

        int k=0;
        //printf("Valor de K: %d y valor de Tamaño: %d\n",k,tamannio);
        for (k; k<tamannio; k++){//Recorre la memoria.
            //printf("Recorre la memoria\n");

            if (memory[k].state == 0){//Si está vacio.

                int e=0;
                for(e;e<cantidad;e++){
                    if( (k+e)==tamannio ){//Si se sale de la memoria falla.
                        //printf("Se sale del tamaño de la memoria\n");
                        //Vacía el contenido, lo deja como si no se hubiera agregado nada.
                        for (int p=0; p<tamannio; p++){
                            if(memory[p].PID==PID){
                                memory[p].state = 0;
                                memory[p].PID = 0;
                                memory[p].burst = 0;
                                memory[p].cantPaginas=0;
                                memory[p].seg = NULL;
                            }
                        }
                        break;
                    }
                    //Si está vacio para ese espacio lo coloca.
                    if (memory[k+e].state == 0){
                        //printf("Agrega Temporal\n");
                        memory[k+e].state = 1;
                        memory[k+e].PID = PID;
                        memory[k+e].burst = burst;
                        memory[k+e].cantPaginas=0;
                        memory[k+e].seg = process->seg;
                    }else{//Si está ocupado borra deja los espacios otra vez desocupados.
                        //printf("Borra lo temporales Temporal\n");   
                        for (int j=0;j<e;j++){//Vacía el contenido, lo deja como si no se hubiera agregado nada.
                            memory[k+j].state = 0;
                            memory[k+j].PID = 0;
                            memory[k+j].burst = 0;
                            memory[k+j].cantPaginas=0;
                            memory[k+j].seg = NULL;
                        }break;
                    }
                }
                /*Si salío y las variables son iguales significa que encontró un lugar
                para los espacios de ese segmento, entonces que continue con el siguiente*/
                if(e==cantidad){break;}
            }
        }
        //Si salió y k es igual al tamaño significa que no encontró campo para ese segmento.
        if(k==tamannio){
            //Agrega el proceso a los muertos.
            printf("\t\nPROCESO DENEGADO.\n");
            wait();
            addToBinnacle(process, "\n%i\t\tDenying allocation  \tAllocation  \t\t%s\t\t%i\t\t\t\t\t%i", 0, 1);
            signal();
            //Agregar el proceso muerto al archivo para verlo desde el espia 
            wait();
            addToDeads(process->PID);
            signal();
            //Inserta el PID actual que este buscando
            // y el ultimo que se puso a buscar o 0 si no hay ultimo. 
            //El 0 es para denotar que no hay nadie buscando
            wait();
            cleanFile("Search_process.txt");
            addToSearch(0, process->PID);
            signal();
            break;
        } 
    }
    signal();//Da el paso para accesar memoria 
    if(i==cantSegmentos){
        printf("\t\nPROCESO ASIGNADO.\n");
        //process->state=1;
        //(process,enMemory); //Listo para ejecutar
        wait();
        addToBinnacle(process, "\n%i\t\tMemory  allocation  \tAllocation  \t\t%s\t\t%i\t\t\t\t\t%i", 1, 1);
        signal();

        //Inserta el PID actual que este buscando
        // y el ultimo que se puso a buscar o 0 si no hay ultimo. 
        //El 0 es para denotar que no hay nadie buscando
        wait();
        cleanFile("Search_process.txt");
        addToSearch(0, process->PID);
        signal();
        
        printProcess();
        sleep(process->burst);//Realiza el sleep de ese proceso.
        sacarProceso(process->PID);
        
        wait();
        addToBinnacle(process, "\n%i\t\tMemory  deallocation\tDeallocation\t\t%s\t\t%i\t\t\t\t\t%i", 1, 1);
        signal();

        printf("Borrado...\n");
        printf("\t..................................................\n");
        printProcess();
        printf("\t..................................................\n");
        printf("Proceso Terminado: %d\n",process->PID);

    }
    
}

void *firstFitPagination(){
    /**
     * @brief Algoritmo de asignacion de memoria para procesos paginados
     * 
     */
    wait();
    Node *nodo = dequeue(ready);
    signal();
    saveBlockProcess();
    if(nodo==NULL) return NULL;
    Process *process = nodo->process;

    short PID = process->PID;
    short burst = process->burst;
    short cantPaginas = process->cantPaginas;
    //Inserta el PID actual que este buscando
    // y el ultimo que se puso a buscar o 0 si no hay ultimo. 
    //El 0 es para denotar que no hay nadie buscando
    wait();
    cleanFile("Search_process.txt");
    addToSearch(process->PID, 0);
    signal();

    printf("Cantidad de pags %i \n",cantPaginas);
    //Variable para saber si hay la cantidad necesaria de paginas 
    //que requiere el proceso
    short enoughSpace = 0;
    wait();
   	for (int i=0; i<tamannio; i++){
        if (memory[i].state == 0){
            if (enoughSpace==cantPaginas){break;}
            else{enoughSpace = enoughSpace+1;}
        }
	}
    signal();
    //Variable para contar las paginas del proceso que ya ha asignado en memoria
    //Cuando es igual a cantPaginas, deja de asignar y se sale del ciclo
    //Si entra a este ciclo asigna la memoria, de lo contrario no habia espacio
    short countSpaces = 0;
    printf("Disponible %i \n", enoughSpace);

    if (enoughSpace==cantPaginas){
        //Empieza a asignar memoria y se bloquea el acceso
        wait();
        for (int i=0; i<tamannio; i++){
            if (memory[i].state == 0 ){
                if (countSpaces<cantPaginas){
                    memory[i].PID = PID;
                    memory[i].burst = burst;
                    memory[i].cantPaginas = cantPaginas;
                    memory[i].state = 1;
                    memory[i].seg = NULL;
                    countSpaces = countSpaces+1;
                }
                else{
                    break;
                }
            }   
        }
        //Da el paso para accesar memoria
        signal();

        printf("\t\nPROCESO ASIGNADO.\n");
        //process->state=1;
        //enqueue(process,enMemory); 
        wait();
        addToBinnacle(process, "\n%i\t\tMemory  allocation  \tAllocation  \t\t%s\t\t%i\t\t\t\t\t%i", 1, 0);
        signal();
        //Inserta el PID actual que este buscando
        // y el ultimo que se puso a buscar o 0 si no hay ultimo. 
        //El 0 es para denotar que no hay nadie buscando
        wait();
        cleanFile("Search_process.txt");
        addToSearch(0, process->PID);
        signal();

        printProcess();
        //0 en el archivo de search significa que no hay proceso buscando 
        sleep(process->burst);//Realiza el sleep de ese proceso.
        sacarProceso(process->PID);
        
        wait();
        addToBinnacle(process, "\n%i\t\tMemory  deallocation\tDeallocation\t\t%s\t\t%i\t\t\t\t\t%i", 1, 0);
        signal();
    }
    else{
        wait();                                                                             //success = 0
        addToBinnacle(process, "\n%i\t\tDenying allocation\tAllocation\t\t%s\t\t%i\t\t\t\t\t%i", 0, 0);
        signal();
        //PROCESO MUERE
        //Agregar el proceso muerto al archivo para verlo desde el espia
        wait();  
        addToDeads(process->PID); 
        signal();
        //Inserta el PID actual que este buscando
        // y el ultimo que se puso a buscar o 0 si no hay ultimo. 
        //El 0 es para denotar que no hay nadie buscando
        wait(); 
        cleanFile("Search_process.txt");
        addToSearch(0, process->PID);
        signal();
    }
    
}


void firstFit(Process *process){
    /**
     * @brief Funcion de administracion de la asignacion de la memoria
     * segun sea paginacion o segmetacion llama a la funcion correspondiente.
     */
    if (isSEGMENTATION==0){
        firstFitPagination(process);
    }
    else{
        firstFitSegmentacion(process);
    }
}

void *buscarProcesosEnReady(){
    /**
     * @brief Funcion que ejecuta un hilo para verificar si hay algo en la cola del ready
     * para enviar a los procesos a colocarse en algun espacio libre.
     */
    pthread_t hiloAsignacion;
    
    while(1){
        //Mientras haya algo en la cola del ready
        if (ready->length>0){
            if (isSEGMENTATION==0){
                pthread_create (&hiloAsignacion, NULL, firstFitPagination, NULL);
            }
            else{
                pthread_create (&hiloAsignacion, NULL, firstFitSegmentacion, NULL);
            }
        }
    }
 
}



int main(int argc, char *argv[]) 
{

    /*
    COMPILAR EL PROGRAMA
    gcc Producer.c -o p2  -pthread
    EJECUTAR EL PROGRAMA, 1 pag y 2 seg
    ./p2 [1|2]
    */

    srandom(time(NULL));//Establecer una raíz

    if (argc<2){printf("ERROR: Debe ingresar el tipo de simulación.\n"); return 0;}

    int tipo = atoi(argv[1]);

    pthread_t hiloCreador;//Hilo para el proceso que crea los procesos.
    pthread_t hiloBuscador;//Hilo de la funcion que se encarga de que los procesos soliciten y hagan la busqueda de su espacio.
    //pthread_t hiloEjecucion;//Hilo de la funcion que se encarga de que los procesos soliciten y hagan la busqueda de su espacio.

    //1:Paginacipn  2:Segmentacion
    if(tipo==1) {isSEGMENTATION=0;}
    else if (tipo==2){isSEGMENTATION=1;}
    else{printf("ERROR: Debe ingresar un modo de simulación válido (1-2).\n"); return 0;}

    //-------------------------------------------------------------------------------------------------------------------------
    //Obtener la llave de la memoria
    key_t memoryKey = getKey(100);
    tamannio = getSize();
    //Obtener el id de la memoria segun clave
    int memoryId = createMemory(memoryKey,tamannio);
    printf("Id de memoria: %i\n",memoryId);
    //Obtener la memoria con shmat
    memory = getMemory(memoryId);

    //Se crea la cola del ready, finished y dead
    ready = createQueue();
    finished = createQueue();
    dead = createQueue();
    //enMemory = createQueue();

    

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

    //-------------------------------------------------------------------------------------------------------------------------
    vaciarMemoria();  //ESTO SOLO CUANDO EL PROGRAMADOR LO NECESITA
    
    cleanFile("Finished_process.txt");

    cleanFile("Block_process.txt");

    cleanFile("Dead_process.txt");

    pthread_create (&hiloCreador, NULL, generarDatos, NULL);
    pthread_create (&hiloBuscador, NULL, buscarProcesosEnReady, NULL);

    pthread_join(hiloCreador, NULL);//Para que el main espere hasta que el creadorProcesos acabe y no se termine el programa.
    pthread_join(hiloBuscador, NULL);
    
    printf("\n\tSTATUS: El creador de proceso se ha detenido.\n");
    return 0;

    
}