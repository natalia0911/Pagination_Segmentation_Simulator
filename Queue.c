#include <stdio.h>
#include <stdlib.h>


typedef struct Node {
    Process *process;
    struct Node *next;

}Node;

//Constructor 
Node * createNode(Process *process){
    Node * n = malloc(sizeof(Node));
    n->next = NULL;
    n->process = process;

}

typedef struct Queue {
    Node *first;
    int length;

}Queue;

//Constructor 
Queue * createQueue(){
    Queue * q = malloc(sizeof(Queue));
    q->first = NULL;
    q->length = 0;
    return q;
}

void enqueue (Process *p, Queue *q){
	if (q->length==0){
        q->first = createNode(p);
        q->length+=1;
        printf("Process enqueued!\n");
    }

    else 
    {
        //Referencia al primero
        Node* actual = q->first ;
        while (actual->next != NULL)
        	actual = actual->next; 
              
        Node* new = createNode(p);
        //Se le asigna al next del penunltimo
        actual->next = new;
        q->length+=1;
        printf("Process enqueued!\n");
    }
}



Node *dequeue (Queue *q){

    if (q->length==0){
		return NULL;
    }
    else{
        //Referencia al primero
        Node* deleted = q->first;    
        //El primero ahora es el segundo
		q->first = q->first->next;
        //Se desenlaza el nodo
		deleted->next = NULL;
        //Retorna el borrado
        printf("Process dequeued!\n");
        q->length-=1;
		return deleted;
    }
 

}



void printInfo(Queue *q, char *tipo){

    Node *temp = q->first;
    //Tipo es muerto, terminado, o ready
    printf("Tipo     PID\n");
    while(temp!=NULL){
        //Imprime el PID 
        printf("-------------------------------------\n");
        printf("%s      %i\n",tipo,temp->process->PID);
        temp = temp->next;
    }

}

void printQueue(Queue *q, int tipo){
    Node *temp = q->first;

    while(temp!=NULL){
        printf("-----------------------------------------------------\n");
        //Tipo es 1 segmetacion 0 es Paginacion
        if(tipo){//Segmentiación
            printf("PID: %i Burst: %i\n",temp->process->PID,temp->process->burst);
            printf("Cantidad Paginas:%i\n",temp->process->seg->cantidad);
        }else{//Paginación.
            printf("PID: %i Burst: %i\n",temp->process->PID,temp->process->burst);
            printf("Cantidad Paginas:%i\n",temp->process->cantPaginas);
        }
        temp = temp->next;
    }   
    printf("-----------------------------------------------------\n");

}


