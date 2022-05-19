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

sem_t semPID;
short PID_AutoIncrement=0;

//Flag que me indica que tipo de algoritmo estoy usando.
int isSEGMENTATION = -1;

//Flag que me indica que tipo de algoritmo estoy usando.
int isRUNNING = 1;

//Colas para que luego el espia acceda
Queue *ready;
Queue *finished;
Queue *dead;




void generarDatos(){
    short burst;
    short espera;
    short PID;
    short cantidadPagSeg;
    short espaciosSeg;

    Process *process;

    pthread_t hiloAgregar;   

    //Usar un flag para detenerlo.
    while(isRUNNING){//De momento usa esa condición para hacer las pruebas

        //ESPERA ENTRE minEspera A maxEspera
        espera = random() % ((MAXTIME) - (MINTIME)+1) + (MINTIME);

        //Usa la variable para pausar el hilo.
        usleep (espera*MICROSECONDOS);
        
        burst = random() % ((MAXBURST) - (MINBURST)+1) + (MINBURST);

        if (isSEGMENTATION==0){//Paginación
            //Ovtiene la cantidad de paginación.
            cantidadPagSeg = random() % ((MAXPAGES) - (MINPAGSEGESP)+1) + (MINPAGSEGESP);

            //Región critiga para obtener el IP del proceso.
            sem_wait(&semPID);
            PID=PID_AutoIncrement++;//Lo obtiene y lo incrementa.
            sem_post(&semPID); 

            process = createProcessPag(PID, burst, cantidadPagSeg);

        }else if (isSEGMENTATION==1){//Segmentación.
            //Obtiene la cantidad de segmentos
            cantidadPagSeg = random() % ((MAXSEGMENTS) - (MINPAGSEGESP)+1) + (MINPAGSEGESP);

            //Región critiga para obtener el IP del proceso.
            sem_wait(&semPID);
            PID=PID_AutoIncrement++;//Lo obtiene y lo incrementa.
            sem_post(&semPID); 

            process = createProcessSeg(PID, burst, cantidadPagSeg);

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
        //pthread_create (&hiloAgregar, NULL, agregarProceso, &process);
            
    }
    //pthread_join (hiloAgregar, NULL );
    return;
}


int main(int argc, char *argv[]) 
{

    /*
    COMPILAR EL PROGRAMA
    gcc Producer.c -o p2  -pthread
    EJECUTAR EL PROGRAMA
    ./p2 [1|2]
    */

    srandom(time(NULL));//Establecer una raíz

    if (argc<2){printf("ERROR: Debe ingresar el tipo de simulación.\n"); return 0;}

    int tipo = atoi(argv[1]);

    pthread_t hiloCreador;//Hilo para el proceso que crea los procesos.
    pthread_t hiloBuscador;//Hilo de la funcion que se encarga de que los procesos soliciten y hagan la busqueda de su espacio.

    printf("Id de memoria: %s\n",tipo);
    
    //1:Paginacipn  2:Segmentacion
    if(tipo==1) {isSEGMENTATION=0;}
    else if (tipo==2){isSEGMENTATION=1;}
    else{printf("ERROR: Debe ingresar un modo de simulación válido (1-2).\n"); return 0;}

    //Obtener la llave de la memoria
    key_t memoryKey = getKey(100);
    int tamannio = getSize();
    //Obtener el id de la memoria segun clave
    int memoryId = createMemory(memoryKey,tamannio);
    printf("Id de memoria: %i\n",memoryId);
    //Obtener la memoria con shmat
    Process *memory = getMemory(memoryId);

    //Se crea la cola del ready, finished y dead
    ready = createQueue();
    finished = createQueue();
    dead = createQueue();

    //Inicia el semaforo para el ID
    sem_init(&semPID, 0, 1);

    pthread_create (&hiloCreador, NULL, generarDatos, NULL);
    //pthread_create (&hiloBuscador, NULL, buscarProcesos, NULL);

    pthread_join(hiloCreador, NULL);//Para que el main espere hasta que el creadorProcesos acabe y no se termine el programa.
    //pthread_join(hiloBuscador, NULL);

    printf("\n\tSTATUS: El creador de proceso se ha detenido.\n");
    return 0;

    
}