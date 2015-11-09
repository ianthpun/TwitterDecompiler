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
#include <unistd.h>
#include <type.h>

#define Want_Debug

void ProcessCleanup(int ProcessCount, int ProcessArr[]);
char * CurrTime(time_t ltime);

time_t ltime;
int* pidstatus;
int CoreNumber;

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
//
CoreNumber = sysconf(_SC_NPROCESSORS_ONLN);

// get first line to see algorithm choice.
fgets(str, 2050, input);

str = strtok(str, "\n");
switch(str){
	case 'round robin':
		rrdecrypt(input, CoreNumber);
		break;
	case 'fcfs':
//		fcfsdecrypt(input, CoreNumber);
		break;
	default:
		printf("Algorithm chosen is not supported. Exiting. \n");
		exit(1);
}















rrndecrypt(FILE *input, int CoreNumber){
 
int ProcessCount = 0;
fd_set rfds;
fd_set wfds;
int ProcessTotal = CoreNumber-1;
int ProcessArr[ProcessTotal];
FD_ZERO(&rfds);
FD_ZERO(&wfds);
 
 
int EncryptionDirectoryPipe[ProcessTotal][2];
int ProcessReadyPipe[ProcessTotal][2];
char* ProcessStatus;
char directorybuffer[2050];
int RRcount = 0;
char* directory[2050];
 
for(int i = 0; i < ProcessTotal; i++){
 
        pipe(EncryptionDirectoryPipe[i]);
        pipe(ProcessReadyPipe[i]);
 
        int ChildID = fork();
        if (ChildID){           // parent process
        FD_SET(ProcessReadyPipe[i][1],&wfds);
        FD_SET(EncryptionDirectoryPipe[i][1],&wfds);
        FD_SET(ProcessReadyPipe[i][0],&rfds);
        FD_SET(EncryptionDirectoryPipe[i][0],&rfds);
        // close the writing end of the pipe, we willl be reading from the child for its status
        close(ProcessReadyPipe[i][1]);
        // close reading end, we will be writing to the child processes
        close(EncryptionDirectoryPipe[i][0]);
        ProcessArr[i] = ChildID;
}
 
 
 
 
        else if (ChildID == 0){         // child process
        // close the reading end of the pipe, we will be writing to the parent
        close(ProcessReadyPipe[i][0]);
        // close writing end, we will be reading the directorys in from the parent
        close(EncryptionDirectoryPipe[i][1]);
        read(EncryptionDirectoryPipe[i][0], directorybuffer,sizeof(directorybuffer));
        printf("ChildID:%i with this line: %s\n", getpid(),directorybuffer);
        exit(0);
        }
}
 
 
// children processes are instantiated
 
while(fgets(directory, 2050, input)){
if(RRcount == (ProcessTotal))
        RRcount = 0;
write(EncryptionDirectoryPipe[RRcount][1], str, strlen(str));
RRcount++;
}

exit();

}


/*

fcfsdecprypt(FILE *input, int CoreNumber){
int ProcessCount = 0;
while(processcount<CoreNumber -1){

	
}





}*/
















