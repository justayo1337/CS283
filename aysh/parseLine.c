/* Contains the function to parse input from the user as expected with redirections and the likes.*/
#include <sys/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "aysh.h"
#include <sys/wait.h>


void redirection(char* buf){
    /* function to catch and open all redirections in the cmdline and returns nothing*/
    char * tmp = malloc(BUFSIZE);
    int cnt = 0;
    char * s = tmp;
    char * tail;
    strcpy(tmp,buf);
    
    while (1){
        ++tmp;
        if (*tmp == '>' && *(tmp+1) == '>'){
            //had to malloc in order to use cleanStr function
            char * file  = malloc(BUFSIZE);
            //starting from the second character after
            tail = tmp +2;
            cnt = 0;
            
            //run untill we hit the end of the line of input or we hit another flag
            while (*tail != '>' && *tail != '<' && *tail != '\n' && (strcmp(tail,"") != 0)){
                ++tail;
                ++cnt;
            }
            strncpy(file,tmp + 2,cnt);
            file = cleanStr(file);
            fd[1] = open(file,O_APPEND | O_WRONLY | O_CREAT,00664) ;
            if (fd[1] < 0){
                perror(file);
                close_fd(fd);
                longjmp(env,1);
            }
            //change the positon for the next iteration of the loop
            tmp = --tail;
        }else if (*tmp == '>'){
            char * file  = malloc(BUFSIZE);
            tail = tmp +1;
            cnt = 0;
            while (*tail != '>' && *tail != '<' && *tail != '\n' && (strcmp(tail,"") != 0)){
                ++tail;
                ++cnt;
            }
            strncpy(file,tmp + 1,cnt);
            file = cleanStr(file);
            fd[1] = open(file,O_WRONLY | O_TRUNC | O_CREAT,00664);
            if (fd[1] < 0){
                perror(file);
                close_fd(fd);
                longjmp(env,1);
            }
            tmp = --tail;
        } else if (*tmp == '<'){
            char * file  = malloc(BUFSIZE);
            tail = tmp +1;
            cnt = 0;
            while (*tail != '>' && *tail != '<' && *tail != '\n' && (strcmp(tail,"") != 0)){
                ++tail;
                ++cnt;
            }
            strncpy(file,tmp+1,cnt);
            file = cleanStr(file);
            fd[0] = open(file,O_RDONLY);
            if (fd[0] < 0){
                perror(file);
                close_fd(fd);
                longjmp(env,1);
            }
            tmp = --tail;
        }else if (*tmp == '\n' || (strcmp(tmp,"") == 0)){
            break;
        }       
    }
    free(s);
}

void parseLine(char* buf,char* toks[],int toclose[][2],int ccnt){
    /* function that parses any input line and returns nothing*/
    int n ,i;
    char * p;

    //to be used as a buffer for individual chunks
    char* toks2[MAXTOKS];
    // for all the redirection funtions
    if (strchr(buf,'|')){
        pipeline(buf, toks);
    }else if (strchr(buf,';') ){
        n = tokenize(buf,toks,";");
        for (i = 0; i < n; i++){
            parseLine(toks[i],toks2,NULL,-1);
        }
    }else if (strstr(buf,">>") || strchr(buf,'>') || strchr(buf,'<')){
        redirection(buf);
        tokenize(buf,toks,"><");
        tokenize(toks[0],toks2," \t\n");
        run(toks2,fd,toclose,ccnt);
    }else{
        n = tokenize(buf,toks," \t\n");
        run(toks,fd,toclose,ccnt);
    }

    close_fd(fd);
}

void close_fd(int * fd){
    /* close all open fds after running the command.*/
    if (fd[0] != -1){
        close(fd[0]);
    }
    if (fd[1] != -1){
        close(fd[1]);
    }
}

void pipeline(char* buf,char* toks[] ){
    int n,i,a;
    //int fds[2];
    char* toks2[MAXTOKS];
    int cpid;

    n = tokenize(buf,toks,"|");
    int fd1[n-1][2];
    int toclose[n*2];
    
    for (i = 0; i<n-1; i++){
        if (pipe(fd1[i]) <0){
            printf("err....\n");
            exit(1);
        }
    }
    for (i = 0; i < n ; i++){
        int cnt;
        cnt = 0;
    
        if (i != 0 && i!=n-1){
            fd[0] = fd1[i-1][0];
            fd[1] = fd1[i][1];
        }else if (i == n-1){
            fd[0] = fd1[i-1][0];
            fd[1] = -1;
        }else{
            fd[1] = fd1[i][1];
            fd[0] = -1;
        }
        parseLine(toks[i],toks2,fd1,n-1);
    }
    //CLOSE All pipes in the parent process since not needed
	for (i = 0; i<n-1; i++){
        close_fd(fd1[i]);
    }
}
