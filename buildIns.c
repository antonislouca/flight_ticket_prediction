#include "ucysh.h"


/*change working directory based on the given directory

*/
void changeDir(char *newdir){
   // printf("dir %s",newdir);
    if(chdir(newdir)!=0){
        perror("Not a valid directory");
    }
}


// /*print working directory*/
// int  printPwd(){
    
//     char * buf=getcwd(NULL,0);
//     if(buf!=NULL){
//     write(STDOUT_FILENO,buf,sizeof(buf));
//      write(STDOUT_FILENO,"\n",1);
//     free(buf);
//     }
//     else{
//         perror("Something went wrong with reading current directory");
//         exit(-1);
//     }
//     return STDOUT_FILENO;
    
// }



/*
A version of echo build in fucntion can print environmental variables
the hostname, and random values and any given message
*/
void Echo(char **args,int length){

    if(args[1]==NULL){ //called echo 
        write(STDOUT_FILENO," \n",3);
        return;
    }
    
    //if it is environmental
        if(args[1][0]=='$'){//is environmental
           if(strlen(args[1])==1){
               perror("let wrong syntax appropriate syntax varname=value");
               return;
           }
            if(strcmp(args[1],"$RANDOM")==0){ //ASKS for a random number
                                           //if it is random ==> generate random numbers
                srand(time(0));
              int random=  rand()% 32768;
             
            
              char buf[35]={'\0'};
              sprintf(buf,"%d\n",random);
              write(STDOUT_FILENO,buf,strlen(buf));
            }else if(strcmp(args[1],"$HOSTNAME")==0){
                  char *envVar=getenv("HOSTNAME");
                  if(envVar==NULL){
                      write(STDOUT_FILENO," \n",3);
                      return;
                  }else{
                      write(STDOUT_FILENO,envVar,strlen(envVar));
                      write(STDOUT_FILENO,"\n",2);
                  }
            }else{

                //   char *temp=strchr(args[1],'$');
                //   temp=temp++;
                args[1]++;
                 // printf("args %s\n",args[1]);
                  char *envVar=getenv(args[1]);
                 // printf("env %s\n",envVar);

                  if(envVar==NULL){
                      write(STDOUT_FILENO," \n",3);
                      return;
                  }else{
                      write(STDOUT_FILENO,envVar,strlen(envVar));
                      write(STDOUT_FILENO,"\n",2);
                  }
                  
            }

        }else{  //its a normal string so we just print it

             int size=0;
                //calculate string size
             for(int i=1;i<length;i++){
                    size+=strlen(args[i]);
             }   
             size=size+length+10;    
            char *buff=(char *)calloc(size,sizeof(char));
            for(int i=1;i<length;i++){
          if(args[i]!=NULL){
          
             int leng=strlen(args[i]);
               // printf("args: %s,length %d\n",args[i],leng);

              //starts with "
              //char *start=NULL;
              int moved=0;
              if(args[i][0]=='"'){
                    moved=1;
                 // start=args[i]++;3
                 args[i]++;
              }

              //ends with "
                char *next=NULL;
                if(args[i][leng-1]=='"'){
                   next=strchr(args[i],'"');
                   next[0]='\0';
                }
                // printf("SECOND args: %s,length %d\n",args[i],leng);
                strcat(buff,args[i]);
                strcat(buff," ");
                
                // if(next!=NULL){
                //      next[0]='"';
                // }
                if(moved==1){
                    args[i]--;//reset pointer
                }
            }
         }

    

            strcat(buff,"\0");
            write(STDOUT_FILENO,buff,size);
            write(STDOUT_FILENO,"\n",1);
            free(buff);
        }
}


/*
prints the history queue calling the queue fucntion to do so
*/
void mhistory(QUEUE *history){
    printHistory(history);
}

/*
prints all the environmental variables
*/
void setEnv(){
    
    for (int i = 0; __environ[i] != NULL; i++){    
        printf("\n%s", __environ[i]);
    }    
    printf("\n");
}

/*
 kills the process corresponding to given PID. When called with -l
 it prints the signal options
*/
void killingMachine(char **args, int len){ //lol

    if(len==1){
        perror("Kill format error. Use kill <PID> OR kill -l");
        return;
    }

    if(strcmp(args[1],"-l")==0){
        char buf[979]={'\0'};
		sprintf(buf," 1) SIGHUP       2) SIGINT       3) SIGQUIT      4) SIGILL       5) SIGTRAP\n"
                     "6) SIGABRT      7) SIGBUS       8) SIGFPE       9) SIGKILL     10) SIGUSR1\n"
                    "11) SIGSEGV     12) SIGUSR2     13) SIGPIPE     14) SIGALRM     15) SIGTERM\n"
                    "16) SIGSTKFLT   17) SIGCHLD     18) SIGCONT     19) SIGSTOP     20) SIGTSTP\n"
                    "21) SIGTTIN     22) SIGTTOU     23) SIGURG      24) SIGXCPU     25) SIGXFSZ\n"
                    "26) SIGVTALRM   27) SIGPROF     28) SIGWINCH    29) SIGIO       30) SIGPWR\n"
                    "31) SIGSYS      34) SIGRTMIN    35) SIGRTMIN+1  36) SIGRTMIN+2  37) SIGRTMIN+3\n"
                    "38) SIGRTMIN+4  39) SIGRTMIN+5  40) SIGRTMIN+6  41) SIGRTMIN+7  42) SIGRTMIN+8\n"
                    "43) SIGRTMIN+9  44) SIGRTMIN+10 45) SIGRTMIN+11 46) SIGRTMIN+12 47) SIGRTMIN+13\n"
                    "48) SIGRTMIN+14 49) SIGRTMIN+15 50) SIGRTMAX-14 51) SIGRTMAX-13 52) SIGRTMAX-12\n"
                    "53) SIGRTMAX-11 54) SIGRTMAX-10 55) SIGRTMAX-9  56) SIGRTMAX-8  57) SIGRTMAX-7\n"
                    "58) SIGRTMAX-6  59) SIGRTMAX-5  60) SIGRTMAX-4  61) SIGRTMAX-3  62) SIGRTMAX-2\n"
                    "63) SIGRTMAX-1  64) SIGRTMAX\n");
        if(write(STDOUT_FILENO,buf,961)<0){
            perror("error in listing kill options");
        }
        
    }else{

        pid_t pid=(pid_t) atoi(args[1]);
        int ret=kill(pid,SIGTERM);
        if(ret==-1){
            char buf[30]={'\0'};
	    	sprintf(buf,"Process %d could not be killed",pid);
            perror("Process could not be killed");
        }
    }
}

/*
    represents the let command and adds a variable to the environment
    can give multipe variables of type x=7 y=9 and will be added
*/
void insertVariable(char **args,int len){
    

    for(int i=1;i<len;i++){
        char *name, *value,*equals;
        equals=strchr(args[i],'=');

        if(equals==NULL){
            perror("let: wrong syntax appropriate syntax varname=value");
            continue;
        }
        equals[0]='\0';
        value=equals+1;
        if(value[0]==' '){
             perror("let: wrong syntax appropriate syntax varname=value");
             continue;
        }

        name =args[i]; 
        int ret=setenv(name,value,1);

        if(ret==-1){
             perror("Environmental varialble could not be set");
        }

        #ifdef DEBUG
        //     char *check=getenv(name);
        //     printf("name %s, value %s variable got back %s n",name,value,check);
        #endif


    }



}

/*
represnets the read build in fucntionality prints a mesage and the variable given 
and adds the value read to the environment as a variable a=6 where a the 
given variable name 
*/
void readFunct(char **args,int len){
    #ifdef DEBUG
    	for(int i=0;i<len;i++){
		printf("arguments %d, %s \n",i,args[i]);
	}
    #endif
        //len =3 if read -p/-r message
        //len =2 if read message
    if(strcmp(args[1],"-p")==0){
            int size=0;
                //calculate string size
             for(int i=2;i<len-1;i++){
                    size+=strlen(args[i]);
             }    

            char *buff=(char *)calloc(size+10,sizeof(char));

         for(int i=1;i<len;i++){
          if(args[i]!=NULL){
          
             int leng=strlen(args[i]);
              //   printf("args: %s,length %d\n",args[i],leng);
              //starts with "
              //char *start=NULL;
              int moved=0;
              if(args[i][0]=='"'){
                    moved=1;
                 // start=args[i]++;3
                 args[i]++;
              }

              //ends with "
                char *next=NULL;
                if(args[i][leng-1]=='"'){
                   // printf("im here fuck you\n");
                   next=strchr(args[i],'"');
                   next[0]='\0';
                }
                
                strcat(buff,args[i]);
                strcat(buff," ");
                
                if(next!=NULL){
                     next[0]='"';
                }
                if(moved==1){
                    args[i]--;//reset pointer
                }
            }
         }
         //printf("message: %s",buff);
        char *name=args[len-1];
        printf("varName: %s\n",name);
        char buf[979]={'\0'};
        if(read(STDIN_FILENO,buf,sizeof(buf))<0){
            perror("read wrong syntax appropriate syntax: read -p message variable name");
            free(buff);
            return;
        }
        int ret=setenv(name,buf,1);

        if(ret==-1){
             perror("Environmental varialble could not be set");
        }

    free(buff);
    }else{
        perror("read wrong syntax appropriate syntax: read -p message variable name");
    }
    
    // if(len==3){

    // }else if(len==2){
        
        
    // }else{
    //     perror("read wrong syntax appropriate syntax: read (-p) message");
    // }
}

/*
    performs the exec build in function replaces the program 
    with the given command after that the program is distroyed
    when the command given is executed and ended the program is no more.
*/
void Myexec(char **args,int len){

        char **temp=args; 
        temp++;//ignore args[0] which is the exec function
        int ret=execvp(temp[0],temp);
        
        if(ret==-1){
        perror("failed to execute command");
        return;
        }

}

/*
    represents the declare build in fucntion that also adds a variable to the 
    environment read only variables are not supported
*/
void declareVariable(char **args,int len){
    

    for(int i=1;i<len;i++){
        char *name, *value,*equals;
        equals=strchr(args[i],'=');

        if(equals==NULL){
            perror("DECLARE: wrong syntax appropriate syntax varname=value");
            continue;
        }
        equals[0]='\0';
        value=equals+1;
        if(value[0]==' '){
             perror("DECLARE: wrong syntax appropriate syntax varname=value");
             continue;
        }

        name =args[i]; 
        int ret=setenv(name,value,1);

        if(ret==-1){
             perror("Environmental varialble could not be set");
        }

        #ifdef DEBUG
        //     char *check=getenv(name);
        //     printf("name %s, value %s variable got back %s n",name,value,check);
        #endif


    }



}