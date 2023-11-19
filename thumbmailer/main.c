/*
Author: Ayomide Adetunji
UserID: aa4449
Purpose: C program to resize pgm image from stdin and output to stdout
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DIV 300
#define BUFF 10

typedef struct {
	int width;
	int height;
	char** bytes;
} Image;

void resizeImage(Image* input,Image* output, int scale);
int compress(Image* input, int srow, int strow, int scol, int stcol);
void freeMemory(Image* memused);

int main(int argc, char** argv){
	int width,height;
	int max;
	int n;
	int scale;
	int fwidth,fheight;
	Image* input;
	Image* output;
	char buff[BUFF];
	int s;
	int i,j;
	
	width = 0;
	height = 0;
	//take in header and calculate the scale needed to resize to have a width close to 300px
 	n = scanf("P5 %d %d %d", &width, &height, &max);
	if (width == 0 || height == 0 || max == 0){
		fprintf(stderr,"Invalid Input!\n");
		return 2;

	}
	scale = width / DIV;	
	fwidth = width / scale;
	fheight = height / scale;

	//allocate memory and store input image	
	input = malloc(sizeof(Image));
	input -> bytes = malloc(height*sizeof(char*));
	input -> height = height;
	input -> width = width;
	output = malloc(sizeof(Image));
	output -> bytes = malloc(fheight*sizeof(char*));
	output -> height = fheight;
	output -> width = fwidth;
	
	//read in the original image into memory with each row as an array of its columns
	for (i = 0 ; i<height ; i++){
		*((input -> bytes)+i) = malloc(width);
		for (j = 0; j < width; j++){
			*(*((input -> bytes)+i)+j) = (unsigned char) getchar();
		}
	}
	
	//resize image
	resizeImage(input,output,scale);

	//write out new image to stdout
	write(1,"P5\n",3);
	s = sprintf(buff,"%d", fwidth);
	write(1,buff,s);
	write(1," ",sizeof(char));
	s = sprintf(buff,"%d", fheight);
	write(1,buff,s);
	write(1,"\n",sizeof(char));
	s = sprintf(buff,"%d", max);
	write(1,buff,s);
	write(1,"\n",sizeof(char));
	for (i = 0 ; i<fheight ; i++){
		write(1,*((output -> bytes) +i),fwidth);
	}

	//free all the memory used
	freeMemory(input);
	freeMemory(output);
	return 0;
}


void resizeImage(Image* input,Image* output, int scale){
	int srow,strow;
	int scol,stcol;
	int owidth;
	int oheight;
	int nrow;
	int ncol;
	int nbyte ;
	nrow = 0 ;
	ncol = 0;
	srow = 0;
	strow = scale;
	oheight = input -> height;
	owidth = input -> width;

	//take each range of 8 rows and calculate the average of all the pixels there
	// checks go on until the stoprow(strow) is equal to the height of the image. 
	// all increments are by the value of the scaling for the input 
		while(strow <= input -> height){
			scol = 0;
			stcol = scale;
			ncol = 0;
			*((output -> bytes)+nrow )= malloc(output->width);
			while (stcol <= input -> width){
				nbyte = compress(input,srow,strow,scol,stcol);
				*(*((output -> bytes)+nrow )+ncol) = nbyte;
				scol = stcol;
				stcol += scale;
				ncol++;
			}
			srow = strow;
			strow += scale;
			nrow++;
		}		
}


int compress(Image* input, int srow, int strow, int scol, int stcol){
	// this function calculates and returns the avg value of the pixels in the range
	unsigned int total=0;
	unsigned int avg = 0 ; 
	int cnt = 0 ;
	int i,j;
	for(i = srow; i < strow; i++){
		for(j = scol; j < stcol; j++){
			total += *(*((input -> bytes)+i)+j);
			cnt++;
		}
	}
	avg = total / cnt;
	return avg;
}

void freeMemory(Image* memused){
	//free every row that was malloc'ed previously for the image
	for (int i = 0 ; i< memused->height ; i++){
		free(*((memused-> bytes) + i ));
	}
	free(memused-> bytes);
	free(memused);
}