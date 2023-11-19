#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include "cc.h"

long nextPos(int fd);

int
main(int argc, char *argv[])
{
	CComp newcomp;
	FILE *fp;

	if(argc != 7) {
		fprintf(stderr, "Usage: ccadd id/-a(chooses next largest values) name maker cpu year desc\n");
		exit(1);
	}

	fp = fopen("ccdb", "r+");
	if(fp == NULL) {
		if(errno == ENOENT) {
			fp = fopen("ccdb", "w+");
			if(fp == NULL ) {
				perror("ccdb");
				exit(2);
			}
		}
	}
	flock(fileno(fp), LOCK_EX);
	// check for the use of the -a flag and then calculate the next available position
	if (strcmp(argv[1],"-a") == 0){
		newcomp.id = nextPos(fileno(fp));
	}else{
		newcomp.id = atoi(argv[1]) ;
	}

	strncpy(newcomp.name, argv[2], Nname-1);
	newcomp.name[Nname-1] = '\0';

	strncpy(newcomp.maker, argv[3], Nmaker-1);
	newcomp.maker[Nmaker-1] = '\0';

	strncpy(newcomp.cpu, argv[4], Ncpu-1);
	newcomp.cpu[Ncpu-1] = '\0';

	newcomp.year = atoi(argv[5]);
	
	strncpy(newcomp.desc, argv[6], Ndesc-1);
	newcomp.desc[Ndesc-1] = '\0';
	
	fseek(fp, newcomp.id * sizeof(CComp), SEEK_SET);
	fwrite(&newcomp, sizeof(CComp), 1, fp);
	flock(fileno(fp), LOCK_UN);
	fclose(fp);
	exit(0);
}

long nextPos(int fd){
	struct stat *statbuf;
	fstat(fd,statbuf);
	long sizes = totalB  ;
	/* ternary check to either set to the current size of the file or the default is 
	set to the size of the datatype*/
	sizes = (statbuf->st_size == 0) ? sizes :  statbuf->st_size;
	return (int) sizes/totalB;
}
