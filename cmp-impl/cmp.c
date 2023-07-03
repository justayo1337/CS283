#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int compareBytes(char* file1, char* file2,int* totalbytes){
	//printf("%d\n",*totalbytes);	
	int diff = 0;
	int i ;
	int final = (strlen(file2) == strlen(file1)) || (strlen(file1) < strlen(file2)) ?  strlen(file1) :  strlen(file2) ;	
	for (i = 0; i < final ; i++){
		if (file1[i] != file2[i]) {
			diff = *totalbytes + 1;
			return diff;
		}
		*totalbytes += 1;
		//printf("%d\n",*totalbytes);
	}

	return diff;
	}


int main(int argc, char** argv){
	
	FILE* file1;
	FILE* file2;
	char* buff1 = NULL;
	char* buff2 = NULL;
	size_t size1 = 0;	
	size_t size2 = 0;
	int lines = 0;
	int totalbytes = 0;

	if (argc = 2){
			file1 = fopen(argv[1],"r");
			file2 = fopen(argv[2],"r");
		}
	
	if (file1 == NULL ){
		fprintf(stderr, "Error opening file: %s\n",argv[1]);
		return 2;
	}
	
	if (file2 == NULL ){
		fprintf(stderr, "Error opening file: %s\n",argv[2]);
		return 2;
	}

	while((getline(&buff1,&size1,file1)  != -1) && (getline(&buff2,&size2,file2) != -1)){
		int diff = 0;
		diff = compareBytes(buff1,buff2,&totalbytes);
		++lines;
		if (diff !=  0){
			printf("The files differ at line: %d and byte: %d\n",lines,diff);	
			return 1	;	
			}else{
				continue;
				}
		free(buff1);
		free(buff2);
		}


	fclose(file1);
	fclose(file2);
	free(buff1);
	free(buff2);
	buff1 = NULL;
	buff2 = NULL;
 	return 0;
}
