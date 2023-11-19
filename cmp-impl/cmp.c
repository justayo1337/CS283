#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void compareBytes(char*,char*,int*,int*);

int main(int argc, char** argv){
	
	FILE* file1;
	FILE* file2;
	char* buff1 = NULL;
	char* buff2 = NULL;
	size_t size1 = 0;	
	size_t size2 = 0;
	int lines = 0;
	int totalbytes = 0;
	int* diff;
	int f1;
	int f2;
	int retval;
	//int array store differing byte and which files
	diff = malloc(sizeof(int) * 2) ;

	
	if (argc<3){
		fprintf(stderr,"Insufficient arguments!\n");
		return 2;
	} 
	if (argc>3){
		fprintf(stderr,"Too many arguments!\n");
		return 2;
	} 
	else {
		file1 = fopen(argv[1],"r");
		file2 = fopen(argv[2],"r");
		}
	if (file1 == NULL ){
	//	fprintf(stderr, "Error opening file: %s\n",argv[1]);
		perror(argv[1]);
		return 2;
	}
	if (file2 == NULL ){
		perror(argv[2]);	
		return 2;
	}
	
	while(1){
		//get line by line
		f1=getline(&buff1,&size1,file1) ;
		f2=getline(&buff2,&size2,file2);
		if ( f1 != -1 &&  f2!= -1){
			//compare bytes in the file
			compareBytes(buff1,buff2,&totalbytes,diff);
			++lines;
		}
		else if (f2 !=-1 && f1 == -1  ) {
			diff[0] = totalbytes;
			diff[1] = 1;
		}
		else if (f2 == -1 && f1 != -1) {
			diff[0] = totalbytes;
			diff[1] = 2;
		}
		else{
			diff[0] = 0;
			diff[1] = 0;
			retval = 0;
			break;
		}
		//choose output base on the contents of the diff variable
		if (diff[0] !=  0 && diff[1] == 0){
			printf("%s & %s  differ at line: %d and byte: %d\n",argv[1],argv[2],lines,diff[0]);	
			retval = 1;	
			break;
			}
		else if (diff[0] !=  0 && diff[1] == 1){
			printf("EOF in file: %s after line: %d and byte: %d\n",argv[1],lines,diff[0]);	
			retval = 1;
			break;
			}
		else if (diff[0] !=  0 && diff[1] == 2){
			printf("EOF in file: %s after line: %d and byte: %d\n",argv[2],lines,diff[0]);	
			retval = 1;
			break;
			}
		else{
			continue;
			}

	free(buff1);
	free(buff2);
	buff1 = NULL;
	buff2 = NULL;
	}
	//clean up.......
	fclose(file1);
	fclose(file2);
	free(buff1);
	free(buff2);
	free(diff);
	buff1 = NULL;
	buff2 = NULL;
	file1 = NULL;
	file2 = NULL;
	diff = NULL;
 	return retval;
}

void compareBytes(char* file1, char* file2,int* totalbytes,int* result){
	int diff = 0;
	int i ;
	int final;
	int l1 = strlen(file1);
	int l2 = strlen(file2);
	final = (l2 == l1) || (l1 < l2) ?  l1 :  l2 ;	
		
	for (i = 0; i < final ; i++){
		if ( ((l1 < l2)) || ((l2 < l1)) || (file1[i] != file2[i]) ) {
			diff = *totalbytes + 1;
			result[0] = diff;
			//check for subsequence
			if ((file1[l1-1] != '\n' && file2[l1])   ) {
					result[1] = 1;
			}
			else if (file1[l2] && file2[l2-1] != '\n'){
					result[1] = 2;
			}
			else{ 
					result[1] = 0; 
					}	
		}
		*totalbytes += 1;
	}
	result[0] = diff;
	}
