/*
Author: Ayomide Adetunji
UserID: aa4449 
Purpose: a ccmatch program that takes a string as a command line argument and 
prints the information about all 
items where the string is a substring of the 
maker, CPU, or description.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <string.h>
#include "cc.h"


void printMatch();
int main(int argc, char** argv){
    FILE* fp;
    CComp item;
    int n, id;

    if (argc != 2){
        fprintf(stderr,"Enter a query string! Usage: ccmatch query_string\n");
        exit(1);
    }

    fp = fopen("ccdb", "r");
    if (fp == NULL){
        perror("ccdb");
        exit(1);
    }
    id = 1;
    flock(fileno(fp), LOCK_SH);
    while (1){
        n = fread(&item,sizeof(CComp),1,fp);
        if (n > 0) {
            /* check if one of the maker,cpu or description has the argument as one of its substrings */
            if (strstr( item.maker,argv[1])
                || strstr(item.desc,argv[1])
                || strstr(item.cpu,argv[1])){
            printf("\n");
            printf("ID: %d\n", item.id);
			printf("Name: %s\n", item.name);
			printf("Maker: %s\n", item.maker);
			printf("CPU: %s\n", item.cpu);
			printf("Year: %d\n", item.year);
			printf("Desc: %s\n", item.desc);
			printf("----------------\n");
            }
        }
        else{
            break;
        }
        id++;
    }
    flock(fileno(fp), LOCK_UN);
    fclose(fp);
    exit(0);
}