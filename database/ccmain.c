/*
Author: Ayomide Adetunji
UserID: aa4449
Purpose: a ccmain program that handles the frontend for the database.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "cc.h"

#define buf 128
void printMenu();
void add();
void del();
void list();
void edit();
void item();
void year();
void match();


int main(){
    char opts[3];
    
    opts[0] = '\0';
    while(opts[0] != 'q') {
        printMenu();
        printf("Enter an option (Enter 'q' to exit): ");
        fgets(opts,3,stdin);
        switch (opts[0]){
        case 'a':
            add();
            continue;
        case  'b':
            del();
            continue;
        case  'c':
            list();
            continue;
        case  'd':
            edit();
            continue;
        case  'e':
            item();
            continue;
        case  'f':
            year();
            continue;
        case  'g':
            //search for specific words
            match();
            continue;
        case 'q':
            break;
        default:
            continue;
        }
    }
    
}

void printMenu(){
    /* This function is called severally to print out these options to stdout for the user*/
    printf("============ MENU OPTIONS ============\n");
    printf("a. Add item to database\n");
    printf("b. Delete item from database\n");
    printf("c. List items in the database\n");
    printf("d. Edit an item in the database\n");
    printf("e. Display a specific item\n");
    printf("f. Show an item in a specified range of years\n");
    printf("g. Search for specific string\n");
}

void add(){
    /*This function takes in all the required input from the user to create a record in the database for the user and returns nothing*/
    int childpid;
    char id[11];
    char name[Nname];
    char maker[Nmaker];
    char cpu[Ncpu];
    char desc[Ndesc];
    char syear[6];
    int year;

    //get item id
    printf("Enter Item ID to be added(can be empty to use next available position): ");
    fgets(id,11,stdin);
    
    //using this to handle the situation the user does not enter any options
    if (strcmp(id, "\n") == 0){
        strcpy(id,"-a");
    }
    //get name of thing
    printf("Enter a New name: ");
    fgets(name,Nname,stdin);
    if (strcmp(name,"\n") != 0){
        /* used to replace the newline that is collects along with the fgets call and terminate the str
        properly*/
        name[strlen(name)-1] = '\0';
    }
    //get maker of thing
    printf("Enter a new Maker: ");
    fgets(maker,Nmaker,stdin);
    if (strcmp(maker,"\n") != 0){
        maker[strlen(maker)-1] = '\0';    
    }

    //get cpu of thing
    printf("Enter a new CPU: ");
    fgets(cpu,Ncpu,stdin);
    if (strcmp(cpu,"\n") != 0){
        cpu[strlen(cpu)-1] = '\0';
    }

    //get year of creation 
    printf("Enter a new Year: ");
    fgets(syear,6,stdin);

    //get description
    printf("Enter a new Desc: ");
    fgets(desc,Ndesc,stdin);
    if (strcmp(desc,"\n") != 0){
        desc[strlen(desc)-1] = '\0';
    }    
    //Usage: ccadd id/-a(chooses next largest values) name maker cpu year desc
    printf("\n");
    childpid = fork();
    if (childpid == 0){
        execlp("./ccadd","./ccadd",id,name,maker,cpu,syear,desc, NULL);
        exit(1);
    }else{
        wait(NULL);
    }
   
}

void del(){
    /* this deletes a record using the ccdel executable that is compiled along with this file*/
    int childpid;
    char* p;
    char id[11];
    printf("Enter Item ID to be deleted: ");
    fgets(id,11,stdin);
    
    //took this from class to handle new lines properly and replace with the nullbyte
    p = strchr(id,'\n');
    if (p != NULL){
        *p = '\0';
    }
    printf("\n");
    childpid = fork();
    if (childpid == 0){
        execlp("./ccdel","./ccdel",id,NULL);
        exit(1);
    }else{
        wait(NULL);
    }
}

void list(){
    /* using the list executable to list all the existing records in the database*/
    int childpid;
    childpid = fork();
    if (childpid == 0){
        execlp("./cclist","./cclist",NULL);
        exit(1);
    }else{
        wait(NULL);
    }
    
}

void edit(){
    /*edit records in the database using the ccedit executable*/
    int childpid;
    char id[11];
    char* p;

    printf("Enter Item ID: ");
    fgets(id,11,stdin);

    //took this from class to handle new lines properly and replace with the nullbyte
    p = strchr(id,'\n');
    if (p != NULL){
        *p = '\0';
    }

    printf("\n");
    childpid = fork();
    if (childpid == 0){
        execlp("./ccedit","./ccedit",id,NULL);
        exit(1);
    }else{
        wait(NULL);
    }
    
}

void item(){
    /*searches and displays a specific item with id in the database file  using ccitem*/
    int childpid;
    char id[11];
    char* p;
    printf("Enter Item ID: ");
    fgets(id,11,stdin);
    //took this from class to handle new lines properly and replace with the nullbyte
    p = strchr(id,'\n');
    if (p != NULL){
        *p = '\0';
    }
    printf("\n");
    childpid = fork();
    if (childpid == 0){
        execlp("./ccitem","./ccitem",id,NULL);
        exit(1);
    }else{
        wait(NULL);
    }

}

void year(){
    /*lists all records within the year range provided*/
    int childpid;
    char y1[6],y2[6];

    printf("Enter Year 1: ");
    fgets(y1,6,stdin);
    
    printf("Enter Year 2: ");
    fgets(y2,6,stdin);
    childpid = fork();
    if (childpid == 0){
        execlp("./ccyear","./ccyear",y1,y2,NULL);
        exit(0);
    }else{
        wait(NULL);
    }
    
    
}

void match(){
    /*searches for a string with several fields of each record in the database.*/
    int childpid;
    char item[buf];
    char* p;
    printf("Enter string to search (max 127 chars): ");
    fgets(item,buf,stdin);
    
    //took this from class to handle new lines properly and replace with the nullbyte
    p = strchr(item,'\n');
    if (p != NULL){
        *p = '\0';
    }
    childpid = fork();
    if (childpid == 0){
        
        execlp("./ccmatch","./ccmatch",item,NULL);
        exit(1);
    }else{
        wait(NULL);
    }
}
