//Librerías  a utilizar
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "SharedFunctions.c"
#include <semaphore.h>
#include "Queue.c"

//Constantes a utilizar
#define MAXPAGES 10
#define MAXSEGMENTS 5
#define MAXESPACIOS 3
#define MINPAGSEGESP 1

#define MAXBURST 60
#define MINBURST 20

#define MAXTIME 60
#define MINTIME 30

#define MICROSECONDOS 1000000

short PID_AutoIncrement=0;

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




void *generarDatos(){
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
        printInfo(ready,"Ready");

        //ESPERA ENTRE minEspera A maxEspera
        espera = random() % ((MAXTIME) - (MINTIME)+1) + (MINTIME);
        //printf("STATUS: Esperando %d segundos.\n", espera);
        usleep (espera*MICROSECONDOS);//Usa la variable para pausar el hilo.
            
    }
}


void printProcess(){
	for (int i=0; i<tamannio; i++)
	{
		printf("Estado %d\n", memory[i].state);
	}
}


void firstFitPagination(Process *process){
    short PID = process->PID;
    short burst = process->burst;
    short cantPaginas = process->cantPaginas;
    printf("Cantidad de pags %i \n",cantPaginas);
    //short state; //0.En memoria, 1.Ejecutando
    //Variable para saber si hay la cantidad necesaria de paginas 
    //que requiere el proceso
    short enoughSpace = 0;
   	for (int i=0; i<tamannio; i++){
        if (memory[i].state == 0){
            if (enoughSpace==cantPaginas){break;}
            else{enoughSpace = enoughSpace+1;}
        }
	}
    //Varbale para contar las paginas del proceso que ya ha asignado en memoria
    //Cuando es igual a cantPaginas, deja de asignar y se sale del ciclo
    short countSpaces = 0;
    printf("enoughSpace %i \n", enoughSpace);
    if (enoughSpace==cantPaginas){
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
    }
    
    addToBinnacle(process, "\n%i\t\tAsignado memoria\tAsignacion\t\t%s\t\tspace\t\t\t\t%i", 1);
    printProcess();
}


void firstFit(Process *process){
    if (isSEGMENTATION==0){
        firstFitPagination(process);
    }
    else{
        printf("Bestfit segmetation");
    }
}

void *buscarProcesosEnReady(){
    
    while(1){
        //Mientras haya algo en la cola del ready
        if (ready->length>0){
            Node *nodo = dequeue(ready);
            Process *process = nodo->process;
            firstFit(process);
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
    
    //1:Paginacipn  2:Segmentacion
    if(tipo==1) {isSEGMENTATION=0;}
    else if (tipo==2){isSEGMENTATION=1;}
    else{printf("ERROR: Debe ingresar un modo de simulación válido (1-2).\n"); return 0;}

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

    pthread_create (&hiloCreador, NULL, generarDatos, NULL);
    pthread_create (&hiloBuscador, NULL, buscarProcesosEnReady, NULL);

    pthread_join(hiloCreador, NULL);//Para que el main espere hasta que el creadorProcesos acabe y no se termine el programa.
    pthread_join(hiloBuscador, NULL);

    printf("\n\tSTATUS: El creador de proceso se ha detenido.\n");
    return 0;

    
}