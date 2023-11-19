/*
Author: Ayomide Adetunji
UserID: aa4449
Purpose: contains additional functions used to manage the shell
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "aysh.h"

void runcmd(char * cmd[],int * redir,int toclose[][2],int ccnt){
    /* FUNCTION THAT RUNS ANY COMMAND THAT IS NOT THE CD COMMAND*/
    int n,a,i;
    
    if (redir[0] >= 0){
                if (n < 0){
                    perror("stdin");
                }
                dup2(redir[0],0);
    }
    if (redir[1] >=0){
        /*for situations where stdin and stdout need to be changes for redirection*/
                n  = dup2(redir[1],1);
                if (n < 0){
                    perror("stdout");
                }
    }
    //close all other fds to allow for proper functioning of pipes
    if (toclose != NULL){
                for( i = 0; i< ccnt; i++){
                    close_fd(toclose[i]); 
                }
            }
    n = execvp(cmd[0],cmd);
    
    if (n < 0){
        perror(cmd[0]);
    }
}

int tokenize(char * s,char * tok[],char* sep){
    /* tokenizes all input from the user and returns the number of tokens */
    int i;
    i = 0;

    tok[i] = strtok(s,sep);
    while(tok[i] != NULL){
        i++;
        if (i >= MAXTOKS){
            tok[i] = NULL;
        }else{
            tok[i] = strtok(NULL,sep);
        }
    }
    return i;
}

void run(char * toks[],int * redir,int toclose[][2],int ccnt){
    /*deal with each different style of line in the parsed input and set*/
    int childpid;
    int n,i,a ;

    if (toks[0] == NULL){
        return;
    }
    if(strcmp(toks[0],"cd") == 0){
        if (toks[1] == NULL || strcmp(toks[1], "~") == 0){
            toks[1] = getenv("HOME");
        }
        n = runcd(toks[1]);
        if (n < 0){
            perror("cd");
        }
    }else if (strcmp(toks[0],"exit") == 0){
        exit(0);
    }else{
        childpid = fork();
        if (childpid == 0){
            runcmd(toks,redir,toclose,ccnt);
            exit(0);
        }else if (childpid == -1){
            perror("fork");
            exit(1);
        }else{
            wait(NULL);
        }
    }
}

char *  cleanStr(char* str){
    /*function to remove spaces from start and end of filename before creating or using in append/redirection*/
    char * p;
    int n;

    //remove newline chars from the names
    p = strchr(str,'\n');
    if (p != NULL){
        *p = '\0';
    }
    //remove spaces too
    while (*str == ' '){
        str++;
    }
    n = strlen(str)-1;
    while (str[n] == ' '){
        str[n] = '\0';
        --n;
    }
    return str;
}
