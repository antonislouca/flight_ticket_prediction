#include "ucysh.h"

//DEDICATED FOR QUEUE FUNCTIONS

/*
initializes the queue struct to default values
@param QUEUE **queue the pointer of the queue
*/
int init_queue(QUEUE **queue) {
    *queue = (QUEUE *) malloc(sizeof(QUEUE));
    if ((*queue) == NULL) return EXIT_FAILURE;
    (*queue)->head = (*queue)->tail = NULL;
    (*queue)->length = 0;
    return EXIT_SUCCESS;
}

/*
checks if queue is empty
*/
int isempty(QUEUE *q){
    if((q->length)==0) return 1;
    return 0;
}

/*
Enqueues the given value to the queue and first and last 
command counters 
Space for value must be allocated

*/
int enqueue (char *value, QUEUE *q,int firstCmd,int lastCmd)  {
    NODE *p = NULL;
    if (q == NULL){ perror("Queue given is NULL\n");exit(-1);}
    p = (NODE *)malloc(sizeof(NODE));
    if ( p == NULL ) {perror("System out of memory...\n");exit(-1);}
    p->first=firstCmd;
    p->last=lastCmd;
    p->data = value; 
    p->next = NULL;
    if (q->length == 0)q->head = q->tail = p;
    else { // append on end
    q->tail->next = p;
    q->tail = p;
    }
    (q->length)++;
    return EXIT_SUCCESS;
}

/*
Dequeus and returns the retval from the queue given along with the 
two counters firstcmd and lastcmd
@param q the queue
@param reval the pointer to the return value
@param firstcmd 
@param lastcmd

*/
int dequeue(QUEUE *q, char **retval,int *firstCmd,int *lastCmd) {
    NODE *p = NULL;       // copy pointer used for free()
    if ((q == NULL) || (q->head == NULL)) {
        perror("Queue is empty \n");exit(-1);
    }

    p = q->head;
    *firstCmd=q->head->first;
    *lastCmd=q->head->last;
    *retval = q->head->data;
    q->head = q->head->next;
    free(p);
    (q->length)--;
    if (q->length == 0) {
    q->tail = NULL;
    }
return EXIT_SUCCESS;
}

/*
Prints the queue ginen used for history queue

*/
void printHistory(QUEUE *q){
    int i=1;
    NODE *n;
    n=q->head;
    while(n->next!=NULL){
      printf("%d  %s\n",i,n->data);
      n=n->next;
      i++;
    }
    printf("%d  %s\n",i,n->data);
}

/*
prints the queue given used in debugging

*/
void printQ(QUEUE *q){
    int i=0;
    NODE *n;
    n=q->head;
    printf("Queue length %d\n",q->length);
    while(n->next!=NULL){
      printf("Node %d :%s \n",i,n->data);
      n=n->next;
      i++;
    }
    printf("Node %d: %s \n",i,n->data);
}

/*
Frees the queue structure given

*/
void freeQueue(QUEUE *q){
     NODE *p;

    while(q->head!=NULL){
        p = q->head;
        q->head = q->head->next;
         free(p->data);
        free(p);
    }

    free(q);
}
