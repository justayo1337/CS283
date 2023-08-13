/* Contains the function to parse input from the user as expected with redirections and the likes.*/
#include <sys/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aysh.h"

void parseLine(char* buf,char* toks[]){
    int n ;
    char * p;
    //to be used as a buffer for individual chunks
    char* toks2[MAXTOKS];
    int fd[2];
    fd[0]= -1;
    fd[1]= -1;
    /* pipes to be added in the next assignment*/
    if (strchr(buf,'|') ){
        n = tokenize(buf,toks,"|");
        for (int i = 0; i < n; i++ ){
            tokenize(toks[i],toks2," \t\n");
            run(toks2,fd,-1);
        }
    }else if (strchr(buf,'<') && strstr(buf,">>")){
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
    }else if (strstr(buf,">>")){
        n = tokenize(buf,toks,">");
        tokenize(toks[0],toks2," \t\n");
        toks[1] = cleanStr(toks[1]);
        fd[0] = open(toks[1], O_APPEND | O_WRONLY);
        if (fd[0] < 0){
            perror(toks[1]);
        }
        run(toks2,fd,1);
	}else if (strchr(buf,'<') && strchr(buf,'>') ){
        if (strchr(buf,'<') > strchr(buf,'>')) {
            n = tokenize(buf,toks,"><");
            /* to parse the command to be redirected into and out of*/
            tokenize(toks[0],toks2," \t\n");
            
            /* clean up whitespace and newline from file names*/
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
            /* run the command, 0 = stdin , 1 = stdout , 2 = both*/
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

    }else if (strchr(buf,'<') ){
        n = tokenize(buf,toks,"<");
        tokenize(toks[0],toks2," \t\n");
        toks[1] = cleanStr(toks[1]);
        fd[0] = open(toks[1],O_RDONLY);
        if (fd[0] < 0){
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
