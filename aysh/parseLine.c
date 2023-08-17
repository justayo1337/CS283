/* Contains the function to parse input from the user as expected with redirections and the likes.*/
#include <sys/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "aysh.h"
#include <sys/wait.h>

int fd[2];
void redirection(char* buf){
    /* function to catch and open all redirections in the cmdline and returns nothing*/
    char * tmp = malloc(BUFSIZE);
    int cnt = 0;
    char * s = tmp;
    strcpy(tmp,buf);
    
    while (1){
        ++tmp;
        if (*tmp == '>' && *(tmp+1) == '>'){
            //had to malloc in order to use cleanStr function
            char * file  = malloc(BUFSIZE);
            //starting from the second character after
            char * tail = tmp +2;
            cnt = 0;
            //run untill we hit the end of the line of input or we hit another flag
            while (*tail != '>' && *tail != '<' && *tail != '\n'){
                ++tail;
                ++cnt;
            }
            strncpy(file,tmp + 2,cnt);
            file = cleanStr(file);
            fd[1] = open(file,O_APPEND | O_WRONLY | O_CREAT,00664) ;
            if (fd[1] < 0){
                perror(file);
                exit(1);
            }
            //change the positon for the next iteration of the loop
            tmp = --tail;
            //free(file);
        }else if (*tmp == '>'){
            char * file  = malloc(BUFSIZE);
            char * tail = tmp +1;
            cnt = 0;
            while (*tail != '>' && *tail != '<' && *tail != '\n'){
                ++tail;
                ++cnt;
            }
            strncpy(file,tmp + 1,cnt);
            file = cleanStr(file);
            fd[1] = open(file,O_WRONLY | O_TRUNC | O_CREAT,00664);
            if (fd[1] < 0){
                perror(file);
                exit(1);
            }
            tmp = --tail;
            //free(file);
        } else if (*tmp == '<'){
            char * file  = malloc(BUFSIZE);
            char * tail = tmp +1;
            cnt = 0;
            while (*tail != '>' && *tail != '<' && *tail != '\n'){
                ++tail;
                ++cnt;
            }
            strncpy(file,tmp+1,cnt);
            file = cleanStr(file);
            fd[0] = open(file,O_RDONLY);
            if (fd[0] < 0){
                perror(file);
                exit(1);
            }
            tmp = --tail;
            //free(file);
        }else if (*tmp == '\n'){
            break;
        }       
    }
    free(s);
}

void parseLine(char* buf,char* toks[]){
    /* function that parses any input line and returns nothing*/
    int n ;
    char * p;

    //to be used as a buffer for individual chunks
    char* toks2[MAXTOKS];
    fd[0]= -1;
    fd[1]= -1;
    // for all the redirection funtions
    if (strstr(buf,">>") || strchr(buf,'>') || strchr(buf,'<')){
        redirection(buf);
        tokenize(buf,toks,"><");
        tokenize(toks[0],toks2," \t\n");
        run(toks2,fd);
    }else if (strchr(buf,';') ){
        n = tokenize(buf,toks,";");
        for (int i = 0; i < n; i++){
            tokenize(toks[i],toks2," \t\n");
            run(toks2,fd);
        }
    }else{
        n = tokenize(buf,toks," \t\n");
        run(toks,fd);
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
