/*
Author: Ayomide Adetunji
UserID: aa4449 
Purpose: a ccedit program that takes an item number as a command line argument.  
It should retrieve the specified item and for each attribute (not including the item number) 
print out the current value and give the user the option to change it.  
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include "cc.h"

void askUser(CComp * item, CComp * newitem);
int main(int argc, char** argv){
    CComp item,newitem;
	FILE *fp;
	int n, inum;


	if(argc != 2) {
		fprintf(stderr, "Usage: ccedit id\n");
		exit(1);
	}

	fp = fopen("ccdb", "r+");
	if(fp == NULL) {
		perror("ccdb");
		exit(2);
	}
	//LOCK FILE
	flock(fileno(fp),LOCK_EX);
    inum = atoi(argv[1]);
	fseek(fp, inum * sizeof(CComp), SEEK_SET);
	n = fread(&item, sizeof(CComp), 1, fp);
    if(n == 0 || item.id != inum) {
		fprintf(stderr, "No such item %d in database\n", inum);
		exit(3);
	}
    if(n < 0) {
		perror("fread");
		exit(3);
	}

    /* handles the user input*/
    askUser(&item,&newitem);

    fseek(fp, item.id * sizeof(CComp), SEEK_SET);
    fwrite(&item,sizeof(CComp),1,fp);
    flock(fileno(fp),LOCK_UN);
    fclose(fp);
    exit(0);
}

void askUser(CComp * item, CComp * newitem){
    char nname[Nname];
    char nmaker[Nmaker];
    char ncpu[Ncpu];
    char ndesc[Ndesc];
    char syear[6];
    int nyear;
    
    printf("Current Name: %s\n", item->name);
    printf("Enter a New name: ");
    fgets(nname,Nname,stdin);
    if (strcmp(nname,"\n") != 0){
        /* used to replace the newline that is collects along with the fgets call and terminate the str
        properly*/
        nname[strlen(nname)-1] = '\0';
        strncpy(item->name, nname, Nname-1);
    }

    printf("Current Maker: %s\n", item->maker);
    printf("Enter a new Maker: ");
    fgets(nmaker,Nmaker,stdin);
    if (strcmp(nmaker,"\n") != 0){
        nmaker[strlen(nmaker)-1] = '\0';
        strncpy(item->maker, nmaker, Nmaker-1);
    }

    printf("Current CPU: %s\n", item->cpu);
    printf("Enter a new CPU: ");
    fgets(ncpu,Ncpu,stdin);
    if (strcmp(ncpu,"\n") != 0){
        ncpu[strlen(ncpu)-1] = '\0';
        strncpy(item->cpu, ncpu, Ncpu-1);
    }

    printf("Current Year: %d\n", item->year);
    printf("Enter a new Year: ");
    //handle the year as a string and then convert to an integer
    fgets(syear,6,stdin);
    nyear = atoi(syear);
    if (nyear != 0 ){
        item->year = nyear;
    }
   
    printf("Current Desc: %s\n", item->desc);
    printf("Enter a new Desc: ");
    fgets(ndesc,Ndesc,stdin);
    if (strcmp(ndesc,"\n") != 0){
        ndesc[strlen(ndesc)-1] = '\0';
        strncpy(item->desc, ndesc, Ndesc-1);
    }    
    printf("----------------\n");
}