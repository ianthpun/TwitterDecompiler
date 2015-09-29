#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "decrypt.h"


//successful completion of this program will exit with 0


int main (int argc, char *argv[] ) {

//has to be exactly 3 arguments
if (argc != 3){
	printf( "You have put too many/few arguments.");
	exit(-1);
	}

// start file pointers
FILE *input;
FILE *output;
// maximum length of character array is 140, this is the temp variable for it + null variable
char str[141];

// open input and output files
input = fopen(argv[1], "r");
output = fopen(argv[2], "w");

//if input returns 0 the input file is missing
if (input == 0){
	printf("Input file is missing.");
	exit(-2);
}

// create a search table to find characters in order to convert to base 41 
char table[] = " abcdefghijklmnopqrstuvwxyz#.,\'!\?()-:$/&\\";

// run while loop until all lines are done
while (fgets(str, 140, input)){   
	decrypt(str, table, output);
	fprintf(output, "\n");
}
// clean up of I/O files and exit
fclose(input);
fclose(output);
printf("Decryption finished.\n");
exit(0);
}



