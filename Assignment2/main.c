// assingment 2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "decrypt.h"
#define Want_Debug

//successful completion of this program will exit with 0


int main (int argc, char *argv[] ) {

//has to be exactly 2 arguments
if (argc != 2){
	printf( "You have put too many/few arguments.");
	exit(-1);
	}

// start file pointer
FILE *input;

char str[141];
// open input and output files
input = fopen(argv[1], "r");

//if input returns 0 the input file is missing
if (input == 0){
	printf("Input file is missing.");
	exit(-2);
}


// run while loop where it grabs each line from the txt file
while (fgets(str, 140, input)){   

// set char pointers that will point to the input output names according to the 
// strtoken. Only need to run strtok twice since its 2 columns
char *inouttemp;

inouttemp = strtok(str, " ");
char *input = inouttemp;

// get next token
inouttemp = strtok(NULL, " ");
char *output = inouttemp;
 

// run decrypt using input and output params
decrypt(input,output);

}

// clean up of I/O files and exit
fclose(input);
exit(0);
}



