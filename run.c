#include "ucysh.h"
//char **args=NULL;
char *args[MAXLIST];
int lengethOfARGS=0;
int numberOFchildren=0;
int ACTIVE=0;

/*
signal sigchild handler when a regular process finsihes the handler is executed
and does nothing when a zombie process finishes it decreases active processes
counter by one and frees resources
*/
void backgroundHandler() {
	 signal(SIGCHLD,backgroundHandler);
    pid_t kidpid;
    int status;
   // printf("Inside zombie deleter ...\n");
    while ((kidpid = waitpid(-1, &status, WNOHANG)) > 0){
		ACTIVE--;
		#ifdef DEBUG
        printf("Child %ld terminated, active processes %d\n", kidpid,ACTIVE);
		#endif
    }
}

/*
main command execution function checks the command given and executes the 
corresponding command. creates a new process for each command that being
either a redirection or a build in that does not affect the parent process
like history
@param inputPIPE represents from where the comamnd will read if is a piped 
command is the returned value from previous execution
@param first : is the command the first command
@param last : is the commadn the last command 
@param code: commadn code representing commadn type
@param hsitory : the command history queue

way it works		ls | grep |wc -c 
	fd1=command(0,1,0,history) args[0]=ls
	fd2=command (fd1,0,0) args[0] =grep
	fd3=command(fd2,0,1) args[0]=wc 
*/
int command(int inputPIPE, int first, int last,int code,QUEUE *history){
	int fd[2];
	
	#ifdef DEBUG
	for(int i=0;i<lengethOfARGS;i++){
		printf("arguments %d, %s \n",i,args[i]);
	}
	#endif

	if((pipe(fd)==-1)){ //create pipe 1
    perror("failed to create pipe");
    exit(-1);
    }

	int pid = fork(); //fork new process
        if(pid==-1){  //check for fork error

            perror("Error in creating new process");
            return -1;
        }

	if (pid == 0) {	//child code

			//checks for redirection
			int input=-1,output=-1;
			for(int i=0;i<lengethOfARGS;i++){
				if(strcmp(args[i],"<")==0){
					input=i;
				}

				if(strcmp(args[i],">")==0){
					output=i;
				}
			}
			//if() input redirection
			if(input!=-1){
				fd[READ]=open(args[input+1],O_RDONLY,0644);

				if(fd[READ]<0){
				perror("Unable to open input file");
				return -1;
				}
				dup2(fd[READ], STDIN_FILENO);
				args[input]=NULL;
			}else if(output!=-1){  //else if outputredirection
				fd[WRITE]=open(args[output+1],O_WRONLY  | O_TRUNC| O_CREAT,0644);

				if(fd[WRITE]<0){
				perror("Unable to open output file");
				return -1;
				}

				dup2(fd[WRITE], STDOUT_FILENO );
				args[output]=NULL;
			}else{// normal command or pipe

				if (first == 1 && last == 0 && inputPIPE == 0) { //first command of pipe
					//close(fd[READ]); //close read dont need it
					dup2(fd[WRITE], STDOUT_FILENO ); //stdout is now the pipe write
				} else if (first == 0 && last == 0 && inputPIPE != 0) {//is a middle command
					// close(fd[READ]);
					dup2(inputPIPE, STDIN_FILENO); //stdin is now the pipe input which is the previous pipe
					dup2(fd[WRITE], STDOUT_FILENO);
				} else { //last pipe commmand just read from input 
					//close(fd[READ]);
					dup2( inputPIPE, STDIN_FILENO );
				}

			}

			switch (code){ //checks what kind of command to execute
       		case regcommand: //execute regular || piped command 

					write(STDOUT_FILENO,"\n",1);
					//execute this only if is not a build in fucntion

					int ret = execvp( args[0], args);  //EXEC THE COMMAND GIVEN 
        			 //check for error
					if(ret==-1){
        			 perror("failed to execute command");
        			 exit(-1);
        			 }

				break;
			case echo: //echo build in
				Echo(args,lengethOfARGS);	
				exit(0);
				break;
			case myhistory:
				mhistory(history);
				exit(0);
				break;
			case setENV:
				setEnv();//see environmental variables
				exit(0);
				break;
			}

	

	}else{ //PARENT CODE
			numberOFchildren++; //INCREASE THE NUMBER OF CHILDREN
	
	}
	//close input pipe if is not zero
	if (inputPIPE != 0) //if input pipe is not standard input close 
		close(inputPIPE);
 
	// close output side of pipe 
	close(fd[WRITE]);
	
	if (last == 1)
		close(fd[READ]);

	return fd[READ]; //return read size of pipe so next command can read the input from it
}


/*
	fucntion that executes the commands of type ./program &
	and commands run in the background
*/
void backgroundExec(){
	int pid =fork();
	if(pid==0){ //child code 
	 //printf("background process\n");
	 args[lengethOfARGS-1]=NULL;
	 int ret = execvp( args[0], args);  //EXEC THE COMMAND GIVEN 
        			 //check for error
	  if(ret==-1){
       perror("failed to execute command");
       exit(-1);
       }

	}else{ //parent code
		ACTIVE++;
	}

}


/*
performs the execution of all the commands in command queue
and add executed commands in history queue. If commands given and active 
processes exceed 10 then the program rejects commands and prints insufficient
resources. the function continues untill given queue is empty.

*/
void  commandExecutioner(QUEUE *q,QUEUE *history){
	 
	int FDinput =0;
	char *cmd=NULL;
	int first=0,last=0,code=0;

		#ifdef DEBUG
		printf("ACTIVE PROCESSES %d \n",ACTIVE);
		#endif

		//check for insufficient resources plus the number of new commands
		//which is equal to the number of elemetns in the command queue
		if(q->length+ACTIVE>10){
			char buf[30]={'\0'};
			sprintf(buf,"Insufficient Resources\n");
			write(STDOUT_FILENO,buf,strlen(buf));
			return ;
		}

		#ifdef DEBUG
		printQ(q); //for debugging
		#endif

	 while (isempty(q)!=1){
		 	dequeue(q,&cmd,&first,&last);

			#ifdef DEBUG
			printf("Command %s length %d first: %d, last %d \n",cmd,(int)strlen(cmd),first,last);
			#endif

		char *tempcmd=calloc(strlen(cmd)+10,sizeof(char)); //make memory alocation
		strcpy(tempcmd,cmd);//copy command going to be executed
		enqueue(tempcmd,history,0,0); //add to command history

		formatCommand(cmd); 	//format command remove whitespace and add NULL
			
		if (args[0] != NULL) { 
			// int bg=0;
		if(strcmp(args[lengethOfARGS-1],"&")==0){
				backgroundExec();
		}else{

		
			code=lookUPmatch(args[0]);
			#ifdef DEBUG
			printf("Command Code: %d\n",code); //CODE representing command
			#endif
			if(code ==Exit){
				cleanup();
				free(cmd); //free command array that was dequeued
       		    exit(0); //exit program 

			}else if(code == cd){  //change dir
					changeDir(args[1]);

			}else if(code==KILL){ //kill process
					killingMachine(args,lengethOfARGS);

			}else if(code==LET){  //insert variable
					insertVariable(args,lengethOfARGS);

			}else if(code==readSub){
					readFunct(args,lengethOfARGS); //read and place in variable or print

			}else if(code==execute){
					Myexec(args,lengethOfARGS); //replace current program with given instruction
			}else if(code==declare){
					declareVariable(args,lengethOfARGS);
			}else{
				FDinput= command(FDinput, first, last,code,history); //return fildescriptor for next child to 
    				                                     //implement pipe mechanism
				// waitchildren(); //wait for spawned processes to finish
				// numberOFchildren=0;
			}	
		}
			for(int i=0;i<lengethOfARGS;i++){
				free(args[i]);
			}

		 free(cmd); //free command array that was dequeued
}

	
}
waitchildren(); //wait for spawned processes to finish
numberOFchildren=0; //ZERO CHILDREN
}


/*
Command that handles the wait process of each forked process
*/

 void waitchildren(){
     int status;

	 #ifdef DEBUG
	 printf("\nchildren nwaiting: %d\n",numberOFchildren);
	 #endif

	for (int i = 0; i < numberOFchildren; i++) {
		wait(&status); 
	}

	

}

/*
Function that splits given string into separate commands and adds them to 
queue. if the string is a piped command where each pipe is, is a different 
enry to queue
*/

void splitCommands(char * cmd,QUEUE *q){
     
		int first = 1;
		char* next = strchr(cmd, '|'); //find first pipe 

		while (next != NULL) {
		
			*next = '\0'; //make pipe a terminator char
			char *tempcmd=calloc(strlen(cmd)+10,sizeof(char));
			strcpy(tempcmd,cmd); //copy first pipe command
			strcat(tempcmd," ");

			#ifdef DEBUG
          	printf("Command that was queued %s length %d\n",tempcmd,(int)strlen(tempcmd)); //print command
			#endif

			enqueue(tempcmd,q,first,0);  //enqueue command in command queue
 
			cmd = next + 1; //get past the pipe
			next = strchr(cmd, '|'); //locate next pipe
			first = 0; //is this the first command ? no set to zero
		}

		char *tempcmd=calloc(strlen(cmd)+10,sizeof(char));
		strcpy(tempcmd,cmd); 		//copy last pipe command
		enqueue(tempcmd,q,first,1); //mark last command

		#ifdef DEBUG
		printf("Command that was queued %s length %d\n",tempcmd,(int)strlen(tempcmd)); //print command 
		#endif
}


/*
ignores the white spaces in strng given
*/


char* ignorewhiteSpace(char* cmd){
	while (isspace(*cmd)!=0){
		++cmd;
	} 
	return cmd;
}



/*
 Formats the command as it suits us
*/
void formatCommand(char* cmd){
	
	int i = 0;
	char *command=cmd; //change reference to cmd to keep it intact

	#ifdef DEBUG
	int leg=strlen(cmd);
	printf("formating command... %s Command length %d\n",command,leg);
	#endif

	command = ignorewhiteSpace(command);
	char *next=strtok(command," "); //NOTE changes cmd string

	while (next!=NULL){
		next=ignorewhiteSpace(next);
		char *copy =calloc(strlen(next)+1,sizeof(char)); //allocate space for string
		strcpy(copy,next); //copy string over
		args[i]=copy;

		#ifdef DEBUG
		printf("Printing args array to check: %s\n",args[i]);
		#endif

		i++;
		next=strtok(NULL," ");
	}
		
	lengethOfARGS=i;
	args[i] = NULL;  //BECARE FULL

	#ifdef DEBUG
	printf("formatted command %s length: %d \n",args[0],(int)strlen(args[0]));
	

	for( i=0;i<lengethOfARGS;i++){
		printf("arguments %d, %s \n",i,args[i]);
	}
	#endif

}

/*
	performs a lookup match to the string given and returns the command code
	appropriate for each command 
*/
int lookUPmatch(char *command){
	#ifdef DEBUG
    printf("Comand: %s\n",command);
	#endif

    if(strcmp(command,"exit")==0) {
        return Exit;
     } else if(strcmp(command,"cd")==0){
	 	return cd;
	}
	else if(strcmp(command,"read")==0){
		return readSub;
	}else if(strcmp(command,"echo")==0){
		return echo;
	}else if(strcmp(command,"kill")==0){
		return KILL;
	}else if(strcmp(command,"history")==0){
		return myhistory;
	}else if(strcmp(command,"set")==0){
		return setENV;
	}else if(strcmp(command,"let")==0){
		return LET;
	}else if(strcmp(command,"exec")==0){
		return execute;
	}else if(strcmp(command,"declare")==0){
		return declare;
	}
    else{
        return regcommand;
    }
}

