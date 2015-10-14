// assignment 2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "decrypt.h"
#include <time.h>

#define Want_Debug

void ProcessCleanup(int ProcessCount, int ProcessArr[]);
char * CurrTime(time_t ltime);

time_t ltime;
int* pidstatus;

//successful completion of this program will exit with 0
int main (int argc, char *argv[] ) {

char *inouttemp;

// I've set array size to 1000 to start, but will increase if its needed since its only holding ints, it wont be too large to begin with.
int ArrSize = 1000;
// This dynamic array will hold the pids of all the processes
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
// increase process count and add it to the array.
ProcessCount++;
if (ProcessCount <= ArrSize)
	ProcessArr[ProcessCount-1] = ChildID;
else { 
// if dyn array is too small, we will allocate more room and add to array.
	printf("Increasing allocated array \n");
	ProcessArr = (int*) realloc(ProcessArr, (ArrSize*2)*sizeof(int));
	ArrSize = ArrSize*2;
	ProcessArr[ProcessCount-1] = ChildID;}
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

printf("[%s] Child Process ID #%i created to decrypt %s. \n",CurrTime(ltime),getpid(),input);

// run decrypt using input and output params
decrypt(input,output);
printf("[%s] Decryption of %s complete. Process ID #%i Exiting. \n",CurrTime(ltime),input, getpid());
exit(0);
}

else {
// New Process fails to create, will do a process cleanup for all children made before error.
printf("[%s] Failed to create new process. Exiting.\n", CurrTime(ltime));
ProcessCleanup(ProcessCount, ProcessArr);
exit(1);}

}

// clean up of Processes and I/O files and exit
ProcessCleanup(ProcessCount, ProcessArr);
fclose(input);
free(ProcessArr);
exit(0);
}


// ProcessClean will take the dyn array ProcessArr and call a waitpid on every child, catching any errors.
void ProcessCleanup(int ProcessCount, int ProcessArr[]){
for (int i = 0; i < ProcessCount; i++){
waitpid(ProcessArr[i], &pidstatus,NULL);
// find a way to check if exit is wrong
if (!WIFEXITED(pidstatus) || WIFSIGNALED(pidstatus) || WEXITSTATUS(pidstatus))
	printf("[%s] Child Process ID #%i did not terminate successfully. \n",CurrTime(ltime),ProcessArr[i]);}
}


