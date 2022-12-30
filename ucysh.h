#ifndef UCYSH_H
#define UCYSH_H
//includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <signal.h>
#include<time.h>


//definitions
#define READ  0
#define WRITE 1

#define regcommand 1
#define Exit 2
#define cd 3
#define pwd 4
#define readSub 5
#define echo 6
#define KILL 7
#define myhistory 8
#define setENV 9
#define LET 10
#define execute 11
#define declare 12

#define MAXLIST 512
#define maxbuf 1024



//structs
typedef struct node { 
    char *data; 
    struct node *next;   
    int first;
    int last;
} NODE; 


typedef struct {
    NODE *head;
    NODE *tail; 
    int length;
} QUEUE;

 //FUNCTION DECLARATIONS
    //CORE FUNCTIONS
    void waitchildren();
    int command(int inputPIPE, int first, int last,int code,QUEUE *history);
    void  commandExecutioner(QUEUE *q,QUEUE *history);
    void splitCommands(char * cmd,QUEUE *q);


    //support functions
    int lookUPmatch(char *command);
    char* ignorewhiteSpace(char* cmd);
    void formatCommand(char* cmd);
    void cleanup();
    void backgroundHandler();
    void insertVariable(char **args,int len);
    

    //build in functions
    //int printPwd();
    void changeDir(char *newdir);
    void Echo(char **args,int length);
    void mhistory(QUEUE *history);
    void setEnv();
    void killingMachine(char **args, int len);
    void readFunct(char **args,int len);
    void Myexec(char **args,int len);
    void declareVariable(char **args,int len);

   

    //queue facntions
    int init_queue(QUEUE **queue);
    int enqueue (char *value, QUEUE *q,int firstCmd,int lastCmd);  
    int dequeue(QUEUE *q, char **retval,int *firstCmd,int *lastCmd);
    int isempty(QUEUE *q);
    void printQ(QUEUE *q);
    void freeQueue(QUEUE *q);
    void printHistory(QUEUE *q);

#endif



