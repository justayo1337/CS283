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


int main(int argc, char** argv){
    char buf[BUFSIZE];
    char* toks[MAXTOKS];

    while(1){
        printf("aysh >>> ");
        if (fgets(buf,BUFSIZE,stdin) == NULL){
            printf("\n");
            exit(2);
        }
        /* split entered commands into tokens*/
        if (buf[0] == '\n'){
            continue;
        }
        parseLine(buf,toks);
    }
    return 0;
}

int runcd(char * dir){
    /* run the cd command and return a value with the return code for the chdir command */
    int n;
    n = chdir(dir);
    return n;
}

void parseLine(char* buf,char* toks[]){
    int n ;
    char * p;
    //to be used as a buffer for individual chunks
    char* toks2[MAXTOKS];
    int fd[2];
    fd[0]= -1;
    fd[1]= -1;

    if (strchr(buf,'|') ){
        n = tokenize(buf,toks,"|");
        for (int i = 0; i < n; i++ ){
            tokenize(toks[i],toks2," \t\n");
            run(toks2,fd,-1);
        }
    }else if (strchr(buf,'<') && strstr(buf,">>")){
        char* toks3[MAXTOKS];
        int fd2;
        
        if (strchr(buf,'<') > strstr(buf,">>")) {
            n = tokenize(buf,toks,"><");
            tokenize(toks[0],toks2," \t\n");
            toks[1] = cleanStr(toks[1]);
            toks[2] = cleanStr(toks[2]);
            fd[0] = open(toks[2],O_RDONLY) ;
            flock(fd[0], LOCK_SH);
            fd[1] = open(toks[1],O_APPEND | O_WRONLY);
            flock(fd[1], LOCK_EX);
            if (fd[0] < 0){
                perror(toks[1]);
            }
            if (fd[1] < 0){
                perror(toks[2]);
            }
            run(toks2,fd,2);
            flock(fd[0], LOCK_UN);
            flock(fd[1], LOCK_UN);
        }

        if (strchr(buf,'<') < strstr(buf,">>")) {
            n = tokenize(buf,toks,"><");
            tokenize(toks[0],toks2," \t\n");
            toks[1] = cleanStr(toks[1]);
            toks[2] = cleanStr(toks[2]);
            fd[0] = open(toks[1],O_RDONLY);
            flock(fd[0], LOCK_SH);
            fd[1] = open(toks[2],O_APPEND | O_WRONLY) ;
            flock(fd[1], LOCK_EX);
            if (fd[0] < 0){
                perror(toks[1]);
            }
            if (fd[1] < 0){
                perror(toks[2]);
            }
            run(toks2,fd,2);
            flock(fd[0], LOCK_UN);
            flock(fd[1], LOCK_UN);

        }
        } else if (strstr(buf,">>")){
        n = tokenize(buf,toks,">");
        tokenize(toks[0],toks2," \t\n");
        toks[1] = cleanStr(toks[1]);
        fd[0] = open(toks[1], O_APPEND | O_WRONLY);
        if (fd[0] < 0){
            perror(toks[1]);
        }
        run(toks2,fd,1);

    }else if (strchr(buf,'<') && strchr(buf,'>') ){
        char* toks3[MAXTOKS];
        int fd2;
        
        if (strchr(buf,'<') > strchr(buf,'>')) {
            n = tokenize(buf,toks,"><");
            tokenize(toks[0],toks2," \t\n");
            toks[1] = cleanStr(toks[1]);
            toks[2] = cleanStr(toks[2]);
            fd[0] = open(toks[2],O_RDONLY) ;
            flock(fd[0], LOCK_SH);
            fd[1] = open(toks[1],O_WRONLY | O_TRUNC | O_CREAT,00664);
            flock(fd[1], LOCK_EX);
            if (fd[0] < 0){
                perror(toks[1]);
            }
            if (fd[1] < 0){
                perror(toks[2]);
            }
            run(toks2,fd,2);
            flock(fd[0], LOCK_UN);
            flock(fd[1], LOCK_UN);
        }

        if (strchr(buf,'<') < strchr(buf,'>')) {
            n = tokenize(buf,toks,"><");
            tokenize(toks[0],toks2," \t\n");
            toks[1] = cleanStr(toks[1]);
            toks[2] = cleanStr(toks[2]);
            fd[0] = open(toks[1],O_RDONLY);
            flock(fd[0], LOCK_SH);
            fd[1] = open(toks[2],O_WRONLY | O_TRUNC | O_CREAT,00664) ;
            flock(fd[1], LOCK_EX);
            if (fd[0] < 0){
                perror(toks[1]);
            }
            if (fd[1] < 0){
                perror(toks[2]);
            }
            run(toks2,fd,2);
            flock(fd[0], LOCK_UN);
            flock(fd[1], LOCK_UN);
        }

    } else if (strchr(buf,'<') ){
        n = tokenize(buf,toks,"<");
        tokenize(toks[0],toks2," \t\n");
        toks[1] = cleanStr(toks[1]);
        //printf("%s\n", toks[1]);
        fd[0] = open(toks[1],O_RDONLY);
        if (fd < 0){
            perror(toks[1]);
        }

        run(toks2,fd,0);

    }else if (strchr(buf,'>') ){
        n = tokenize(buf,toks,">");
        tokenize(toks[0],toks2," \t\n");
        toks[1] = cleanStr(toks[1]);
        fd[0] = open(toks[1],O_WRONLY | O_TRUNC | O_CREAT,00664);
        if (fd[0] < 0){
            perror(toks[1]);
        }
        run(toks2,fd,1);

    }else if (strchr(buf,';') ){
        n = tokenize(buf,toks,";");
        for (int i = 0; i < n; i++){
            tokenize(toks[i],toks2," \t\n");
            run(toks2,fd,-1);
        }
    }else{
        n = tokenize(buf,toks," \t\n");
        run(toks,fd,-1);
    }
}


char *  cleanStr(char* str){
    char final[BUFSIZE];
    char * p;
    int n;
    p = strchr(str,'\n');
    if (p != NULL){
        *p = '\0';
    }
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