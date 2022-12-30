#include "ucysh.h"
 char *buf,*outbuf,*copy;
 QUEUE *q,*history;

 /*
Performs a clean up of the program buffers and queue
dallocates space

*/
 void cleanup(){
    free(buf);
    free(history);
    free(copy);
    free(outbuf);
    freeQueue(q);
}

/*
Main fucntion of the program reads input from the user
prints shell curssor and tokenizes commands 
of type ls;cd;pwd 

*/
int main(int argc, char *argv[]){
    
    signal(SIGCHLD,backgroundHandler);
   //signal(SIGCHLD,SIG_IGN);
    buf=(char *)calloc(maxbuf,sizeof(char));
    outbuf=(char *)calloc(30,sizeof(char));
    init_queue(&history);
    
	while (1) {
		/* Print the command prompt arrow*/
          //  printf("%d-ucysh>\n",history->length);
        sprintf(outbuf,"%d-ucysh>",history->length);
		if(write(STDOUT_FILENO,outbuf,30)<0){
            perror("Error while writing");
           continue;
        }
        //fflush(NULL);
        //read command 
        if(read(STDIN_FILENO,buf,maxbuf)<0){
            bzero(buf,maxbuf);
            bzero(outbuf,30);
            continue;
            // sprintf(outbuf,"%d-ucysh>",history->length);

		    //     if(write(STDOUT_FILENO,outbuf,30)<0){
            //         perror("Error while writing");
            //     }
            //perror("Error while reading input");  
        }  
     
    
        if(buf[0]=='\n'||strlen(buf)==2){ //when enter is pressed

            //perror("Invalid command");
            bzero(buf,maxbuf);
            bzero(outbuf,30);
            // free(buf);
            // free(outbuf);
            // buf=(char *)calloc(maxbuf,sizeof(char));
            // outbuf=(char *)calloc(30,sizeof(char));  
			continue;
		}
        
        #ifdef DEBUG
        printf("buffer: %s \n",buf);
        #endif

        char *next=strchr(buf,'\n');
        next[0]='\0';               //remove new line character

        char *temp=buf;
        next=strchr(buf,';');

        #ifdef DEBUG
        printf("Next %s\n",next);
        #endif

        //IF WE HAVE ; in COMMAND
        if(next!=NULL){

         while(next!=NULL){  //tokenize based on ; and then if there is a pipe will be
                                //tokenized later on
            next[0]='\0';

            #ifdef DEBUG
            printf("Next token: %s length: %d\n",temp,(int)strlen(temp));
            #endif

            init_queue(&q); //initialize command queue

            copy=calloc(strlen(temp)+1,sizeof(char));
            strcpy(copy,temp);

            #ifdef DEBUG
            printf("Copy command %s\n",copy);
            #endif
           
            splitCommands(copy,q);          //split commands if they contain pipes 
                                            //and add to queue 
            commandExecutioner(q,history);  //call command executionner


            free(copy);         //free copy of command
            freeQueue(q);       //free command queue
            temp=next+1;
            next=strchr(temp,';'); //locate next ; that delimits commands
           }

        }


            //only one command or there is one last command
            init_queue(&q); //initialize command queue

            copy=calloc(strlen(temp)+1,sizeof(char));
            strcpy(copy,temp);

            #ifdef DEBUG
            printf("Copy command %s\n",copy);     
            #endif  
           
            splitCommands(copy,q);          //split commands if they contain pipes 
                                            //and add to queue 

           commandExecutioner(q,history);  //call command executionner


            free(copy);         //free copy of command
            freeQueue(q);       //free command queue


        //nullify buffers for reuse
        bzero(buf,maxbuf);
        bzero(outbuf,30);

        // free(buf);
        // free(outbuf);
        // buf=(char *)calloc(maxbuf,sizeof(char));
        // outbuf=(char *)calloc(30,sizeof(char));

        
    
	
}

	
	return 0;
}

