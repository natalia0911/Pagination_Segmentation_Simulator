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
        printf("There is nothing in the queue!\n");
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

Node *deleteNode(Process *p,Queue *q){
    //Dado un proceso retorna el nodo en el que se encuentra
    Node* actual = q->first ;

    if (actual->process == p){
        return dequeue (q);
    }
    else{
        while (actual->next->process != p){ 
            actual = actual->next; 
        }
        //Nodo que ando buscando
        Node *temp = actual->next;
        actual->next = temp->next;
        temp->next = NULL;
        q->length-=1;
        printf("Process deleted!\n");
        return temp; 
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
        //Avanza al siguiente nodo
        temp = temp->next;
    }

}


