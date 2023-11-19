/*
Author: Ayomide Adetunji
UserID: aa4449
Purpose: a ccdel program that takes an item number as a command line argument and deletes that item from the database.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include "cc.h"



int main(int argc, char** argv){
    FILE * fp;
    int n, id;
    CComp item;

    if (argc != 2){
        fprintf(stdout,"Enter an item number! Usage: ccdel id\n");
        exit(1);
    }

    id = atoi(argv[1]);

    fp = fopen("ccdb", "r+");
	if(fp == NULL) {
        perror("ccdb");
        exit(2);
	}

    flock(fileno(fp),LOCK_EX);
    fseek(fp,id * sizeof(CComp),SEEK_SET);
    n = fread(&item,sizeof(CComp),1,fp);
    if(n == 0 || item.id != id) {
		fprintf(stderr, "No such item %d in database\n", id);
		exit(3);
	}
    if(n < 0) {
		perror("fread");
		exit(3);
	}

    //printing the old values to be deleted
    printf("Name: %s\n", item.name);
	printf("Maker: %s\n", item.maker);
	printf("CPU: %s\n", item.cpu);
	printf("Year: %d\n", item.year);
	printf("ID: %d\n", item.id);
	printf("Desc: %s\n", item.desc);
    /* make all the members of the struct empty/null and then write back into the same position to replace 
    the old values*/
    item.name[0] = '\0';
    item.maker[0] = '\0';
    item.cpu[0] = '\0';
    item.year = 0;
    item.id = 0;
    item.desc[0] = '\0';

    fseek(fp,id * sizeof(CComp),SEEK_SET);
    fwrite(&item,sizeof(CComp), 1, fp);
    flock(fileno(fp),LOCK_UN);
    fclose(fp);
    exit(0);
}