/*Ian Pun
Student number : 301167944
itpun@sfu.ca
CMPT 300 D100
Instructor: Brian Booth
TA: Scott Kristjanson

Description:
Lyrebird is a program that will analyse and decrypt any encrypted text message that was sent from these "no-so-nice-people". It will use the methods that was discovered by the mathematician:

1) remove every 8th character 
2)change all groups of 6 characters to base 41 and then expand it positionally
3) Run Modular Exponentiation using exponent 1921821779 and mod 4294434817
4) Run the inverse of step 2 to get the decrypted text.


Due to the amount of twitter messages needed to be decrypted, Lyrebird can now run its decryption with multiple processes. It will do this by taking in a list text with directories of 
all the decrypted messages and its designated output directory. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "decrypt.h"
#include <time.h>
//#include "wait.h"
#include "memwatch.h"

#define Want_Debug

void ProcessCleanup(int ProcessCount, int ProcessArr[]);
char * CurrTime(time_t ltime);

time_t ltime;
int* pidstatus;

int main (int argc, char *argv[]) {

//has to be exactly 2 arguments
if (argc != 2){
	printf( "You have put too many/few arguments.\n");
	exit(-1);}

// start file pointer
FILE *input;
// open input file
input = fopen(argv[1], "r");

//if input returns 0 the input file is missing
if (input == 0){
	printf("Input file is missing.\n");
	exit(-2);
}

char *inouttemp;

// I've set array size to 1000 to start, but will increase if its needed since its only holding ints, it wont be too large to begin with.
int ArrSize = 1000;
// This dynamic array will hold the pids of all the processes

ProcessArr = (int*) malloc(ArrSize*sizeof(int)); 

int ProcessCount = 0;

/* I have used a character string of 2050 because we can safely say each directory will be at most 1024 chars. 
 So two directorys + the space inbetween is (1024*2)+1 = 2049, which I figured I'd round up to 2050 just in case. */
char str[2050];

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
        printf("input is %s\n", input);
        printf("output is %s\n", output);
		// run decrypt using input and output params

		printf("[%s] Child Process ID #%i created to decrypt %s. \n",CurrTime(ltime),getpid(),input);
		decrypt(input,output);
		printf("[%s] Decryption of %s complete. Process ID #%i Exiting. \n",CurrTime(ltime),input, getpid());
		free(ProcessArr);
		exit(0);
	}

	else {
		// New Process fails to create, will do a process cleanup for all children made before error.
		printf("[%s] Failed to create new process. Exiting.\n", CurrTime(ltime));
		ProcessCleanup(ProcessCount, ProcessArr);
		free(ProcessArr);
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
	if ((!WIFEXITED(pidstatus)) || (WIFSIGNALED(pidstatus)) || (WEXITSTATUS(pidstatus)))
		printf("[%s] Child Process ID #%i did not terminate successfully. \n",CurrTime(ltime),ProcessArr[i]);}
}


