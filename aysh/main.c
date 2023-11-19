/*
Author: Ayomide Adetunji
UserID: aa4449
Purpose: contains the main base functions for the shell and includes the builtins.
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
#include <sys/file.h>
#include <signal.h>
#include <setjmp.h>

jmp_buf env;
int fd[2];
int main(int argc, char** argv){
    char buf[BUFSIZE];
    char* toks[MAXTOKS];
    //to catch when CTRL-C is run
    signal(SIGINT, handle_signals);
    
    while(1){
        
        setjmp(env);
        fd[0] = -1;
        fd[1] = -1;
        printf("aysh >>> ");
        if (fgets(buf,BUFSIZE,stdin) == NULL){
            printf("\n");
            exit(2);
        }
        /* split entered commands into tokens*/
        if (buf[0] == '\n'){
            continue;
        }
        parseLine(buf,toks,NULL,0);
    }
    return 0;
}

int runcd(char * dir){
    /* run the cd command and return a value with the return code for the chdir command */
    int n;
    n = chdir(dir);
    return n;
}

void handle_signals(){
    printf("\n");
    longjmp(env,1);
}