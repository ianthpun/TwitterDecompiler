// assignment 2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "decrypt.h"
#include <time.h>

#define Want_Debug


/* Use this to optimize later if needed
#include <unistd.h>
sysconf(_SC_NPROCESSORS_ONLN); */ 

//successful completion of this program will exit with 0


int main (int argc, char *argv[] ) {

// i learned this from : 
time_t ltime;
time(&ltime);
char *inouttemp;
int ArrSize = 1000;

int* pidstatus;

int * ProcessArr;
ProcessArr = (int*) malloc(ArrSize*sizeof(int)); 
int ProcessCount = 0;

//has to be exactly 2 arguments
if (argc != 2){
	printf( "You have put too many/few arguments.\n");
	exit(-1);
	}

// start file pointer
FILE *input;

char str[2050];
// open input and output files
input = fopen(argv[1], "r");

//if input returns 0 the input file is missing
if (input == 0){
	printf("Input file is missing.\n");
	exit(-2);
}


// run while loop where it grabs each line from the txt file
while (fgets(str, 2050, input)){   
int ChildID = fork();

// Parent Process
if (ChildID){
// increase process count.
ProcessCount++;
if (ProcessCount <= ArrSize)
	ProcessArr[ProcessCount-1] = ChildID;
else {
	ProcessArr = (int*) realloc(ProcessArr, (ArrSize*2)*sizeof(int));
	ArrSize = ArrSize*2;
}
}


// New Process
else if (ChildID == 0){
// set char pointers that will point to the input output names according to the 
// strtok. Only need to run strtok twice since its 2 columns
inouttemp = strtok(str, " ");
char *input = inouttemp;

// get next token
inouttemp = strtok(NULL, " ");
char *output = inouttemp;
 output = strtok(output, "\n");

printf("[%s] Child Process ID #%i created to decrypt %s. \n",ctime(&ltime),getpid(),input);

// run decrypt using input and output params
decrypt(input,output);
printf("[%s] Decryption of %s complete. Process ID #%i Exiting. \n",ctime(&ltime),input, getpid());
exit(0);
}

// New Process fails to create
else {
printf("[%s] Failed to create new process. Exiting.\n", ctime(&ltime));
exit(-1);
}

}

for (int i = 0; i < ProcessCount; i++){
waitpid(ProcessArr[i], &pidstatus,NULL);
if (!WIFEXITED(pidstatus) || WIFSIGNALED(pidstatus))
printf("[%s] Child Process ID # %i  did not terminate successfully. \n",ctime(&ltime),ProcessArr[i]);
}
// clean up of I/O files and exit
fclose(input);
free(ProcessArr);
exit(0);
}



