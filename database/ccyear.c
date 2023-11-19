/*
Author: Ayomide Adetunji
UserID: aa4449 
Purpose: a ccyear program that takes two years on the command line and prints 
the information about all items whose year 
is included in the range of years specified 
as command line arguments
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include "cc.h"

int main(int argc, char** argv){
    FILE* fp;
    int n;
    CComp item;
    int year1,year2;
    int id;

    if (argc != 3){
        fprintf(stderr,"Need More args! Usage: ccyear startyear endyear\n");
        exit(1);
    }

    year1 = atoi(argv[1]);
    year2 = atoi(argv[2]);
    if (year1 > year2){
        fprintf(stderr,"First arg must be greater than the second!\nUsage: ccyear startyear endyear\n");
        exit(1);
    }
    fp = fopen("ccdb", "r");
    if (fp == NULL){
        perror("ccdb");
        exit(2);
    }
    flock(fileno(fp), LOCK_SH);

    /* loop while the first argument is not greater than the final year in the range
       increment the lower bound and loop through all items in the database for each year in the range
    */
    while (year1 <= year2){
        id =1;
        while(1){
            if (fread(&item,sizeof(CComp),1,fp) > 0) {
                if (item.year == year1){
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
        fseek(fp,0,SEEK_SET);
        year1++;
        
    }
    flock(fileno(fp), LOCK_UN);
    fclose(fp);
    exit(0);
}