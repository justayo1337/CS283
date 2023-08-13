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
#include "aysh.h"
#include <sys/stat.h>
#include <fcntl.h>

void runcmd(char * cmd[],int * redir,int dir){
    /* FUNCTION THAT RUNS ANY COMMAND THAT IS NOT THE CD COMMAND*/
    int n;
    if (redir[0] > 0 && dir > -1  && dir < 2){
                dup2(redir[0],dir);
                close(redir[0]);
    } else if (redir[0] > 0 &&  redir[1] > 0 && dir > -1  && dir == 2){
                printf("here\n");
                dup2(redir[1],dir-1);
                dup2(redir[0],dir-2);
                close(redir[1]);
                close(redir[0]);
    }
    n = execvp(cmd[0],cmd);
    printf("n: %d\n",n);
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

void run(char * toks[],int * redir,int dir){
    /*deal with each different style of line in the parsed input and set*/
    int childpid;
    int n;
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
            runcmd(toks,redir,dir);
            exit(0);
        }else if (childpid < 0){
            perror("fork");
        }else{
            wait(NULL);
        }
    }
}
