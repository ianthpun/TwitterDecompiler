/*Ian Pun
Student number : 301167944
itpun@sfu.ca
CMPT 300 D100
Instructor: Brian Booth
TA: Scott Kristjanson
*/

#include "utilfunc.h"
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include "memwatch.h"


// This will include helper functions for Lyrebird


// This is a helper function written so it will grab the current time and also remove the 
// new line character that is within the time() function call.
char * CurrTime(time_t ltime){
	time(&ltime);
	char * temp =ctime(&ltime);
	// remove the new line characater from the date
	temp = strtok(temp, "\n");
	return temp;
}


// ProcessClean will take the dyn array ProcessArr and call a waitpid on every child, catching any errors.
void ProcessCleanup(int ProcessCount, int ProcessArr[]){
int* pidstatus;
time_t ltime;
for (int i = 0; i < ProcessCount; i++){
    waitpid(ProcessArr[i], &pidstatus,NULL);
    if ((!WIFEXITED(pidstatus)) || (WIFSIGNALED(pidstatus)) || (WEXITSTATUS(pidstatus)))
        printf("[%s] Child Process ID #%i did not terminate successfully. \n",CurrTime(ltime),ProcessArr[i]);}
}



// returns the int specified for the algo selected.
int AlgoCheck(char* algo){

const char FCFS[] = "fcfs";
const char RR[] = "round robin";


	// return 1 for FCFS
	if (strcmp(FCFS,algo) == 0)
	return 1;
	// return 2 for Round Robin
	if (strcmp(RR,algo) == 0)
	return 2;
	// not a supported algorithm
	else return -1;
}
