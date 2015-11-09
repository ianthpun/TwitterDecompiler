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
#include <sys/types.h>

#define Want_Debug

void rrdecrypt(FILE *input, int CoreNumber);
void ProcessCleanup(int ProcessCount, int ProcessArr[]);
void fcfsdecrypt(FILE *input, int CoreNumber);
//char * CurrTime(time_t ltime);
time_t ltime;
int* pidstatus;
int CoreNumber;

int main (int argc, char *argv[]) {

char str[50];
char *strtemp;


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
/*fgets(str, 2050, input);

strtemp = strtok(str, "\n");*/

fcfsdecrypt(input, CoreNumber);
exit(0);

}















void rrdecrypt(FILE *input, int CoreNumber){
 
int ProcessCount = 0;
fd_set rfds;
fd_set wfds;
int ProcessTotal = CoreNumber-1;
int ProcessArr[ProcessTotal];
FD_ZERO(&rfds);
FD_ZERO(&wfds);
 
 
int EncryptionDirectoryPipe[ProcessTotal][2];
int ProcessReadyPipe[ProcessTotal][2];
int ProcessStatus;
char directorybuffer[2050];
int RRcount = 0;
char* directory[2050];
int directorysize;
int ProcessesCreated;
 
for(int i = 0; i < ProcessTotal; i++){
 
        pipe(EncryptionDirectoryPipe[i]);
//      pipe(ProcessReadyPipe[i]);
 
        int ChildID = fork();
        if (ChildID){           // parent process
//      FD_SET(ProcessReadyPipe[i][1],&wfds);
//      FD_SET(ProcessReadyPipe[i][0],&rfds);
        FD_SET(EncryptionDirectoryPipe[i][1],&wfds);
        FD_SET(EncryptionDirectoryPipe[i][0],&rfds);
        // close the writing end of the pipe, we willl be reading from the child for its status
//      close(ProcessReadyPipe[i][1]);
        // close reading end, we will be writing to the child processes
        close(EncryptionDirectoryPipe[i][0]);
        ProcessArr[i] = ChildID;
        printf("child put into ProcessArr: %i\n",ProcessArr[i]);
        ProcessesCreated++;
}
 
 
        else if (ChildID == 0){         // child process
        // close the reading end of the pipe, we will be writing to the parent
 //     close(ProcessReadyPipe[i][0]);
        // close writing end, we will be reading the directorys in from the parent
        close(EncryptionDirectoryPipe[i][1]);

        while(1){

        int dirsize;
        int readstatus = read(EncryptionDirectoryPipe[i][0], &dirsize,sizeof(int));
        
        if (readstatus==0){
        printf("child %i has reach end of read. exiting loop.\n", getpid());
        break;}

        read(EncryptionDirectoryPipe[i][0], directorybuffer,dirsize);
        char *inouttemp = strtok(directorybuffer, " ");
        char *input = inouttemp;
        // get next token
        inouttemp = strtok(NULL, " ");
        char *output = inouttemp;
        output = strtok(output, "\n");

        // run decrypt using input and output params
        printf("[%s] Child Process ID #%i will decrypt %s. \n",CurrTime(ltime),getpid(),input);
        if(decrypt(input,output))
        printf("[%s] Process ID #%i decrypted %s successfully. \n",CurrTime(ltime),getpid(),input);
        else
        printf("Decryption Error. Moving to next decryption.\n");    
        }

        close(EncryptionDirectoryPipe[i][0]);
        return;
        }

        else{
        // Process failed to create.
        ProcessCleanup(i, ProcessArr); 
        fclose(input);
        return;
        }
}
 
 
// children processes are instantiated

int dircount;

while(fgets(directory, 2050, input)){
if(RRcount == (ProcessTotal))
        RRcount = 0;
char* dir = strtok(directory, "\n");
directorysize = strlen(dir);
write(EncryptionDirectoryPipe[RRcount][1], &directorysize, sizeof(int));
write(EncryptionDirectoryPipe[RRcount][1], dir, strlen(dir));
RRcount++;
dircount++;
}

for (int i = 0; i< ProcessTotal; i++){
close(EncryptionDirectoryPipe[i][1]);
}


/*
if (dircount < ProcessTotal){
for (int i = dircount+1; i <= ProcessTotal; i++){
printf("killing unused processes at id: %i \n", ProcessArr[i-1]);
kill(ProcessArr[i], SIGKILL);
}
}
*/

ProcessCleanup(ProcessesCreated, ProcessArr);
fclose(input);
exit(0);

}


void fcfsdecrypt(FILE *input, int CoreNumber){
int ProcessCount = 0;
fd_set rfds;
fd_set wfds;
int ProcessTotal = CoreNumber-1;
int ProcessArr[ProcessTotal];
FD_ZERO(&rfds);
FD_ZERO(&wfds);
struct timeval tv;
tv.tv_sec = 100;
tv.tv_usec = 0; 
 
int EncryptionDirectoryPipe[ProcessTotal][2];
int ProcessReadyPipe[ProcessTotal][2];
int ProcessStatus = 1;
char directorybuffer[2050];
int RRcount = 0;
char* directory[2050];
int directorysize;
int ProcessesCreated;
 
for(int i = 0; i < ProcessTotal; i++){
 
        pipe(EncryptionDirectoryPipe[i]);
        pipe(ProcessReadyPipe[i]);
 
        int ChildID = fork();
        if (ChildID){           // parent process
//        FD_SET(ProcessReadyPipe[i][1],&wfds);
        FD_SET(EncryptionDirectoryPipe[i][1],&wfds);
        FD_SET(ProcessReadyPipe[i][0],&rfds);
        // close the writing end of the pipe, we willl be reading from the child for its status
        close(ProcessReadyPipe[i][1]);
        // close reading end, we will be writing to the child processes
        close(EncryptionDirectoryPipe[i][0]);
        ProcessArr[i] = ChildID;
        printf("child put into ProcessArr: %i\n",ProcessArr[i]);
        ProcessesCreated++;
}
 
 
        else if (ChildID == 0){         // child process
        // close the reading end of the pipe, we will be writing to the parent
        close(ProcessReadyPipe[i][0]);
        // close writing end, we will be reading the directorys in from the parent
        close(EncryptionDirectoryPipe[i][1]);

        while(1){
        int dirsize;
        int readstatus = read(EncryptionDirectoryPipe[i][0], &dirsize,sizeof(int));
        if (readstatus==0){
        printf("child %i has reach end of read. exiting loop.", getpid());
                break;}
        
        read(EncryptionDirectoryPipe[i][0], directorybuffer,dirsize);
        char *inouttemp = strtok(directorybuffer, " ");
        char *input = inouttemp;
        // get next token
        inouttemp = strtok(NULL, " ");
        char *output = inouttemp;
        output = strtok(output, "\n");

        printf("[%s] Child Process ID #%i will decrypt %s. \n",CurrTime(ltime),getpid(),input);
        if(decrypt(input,output))
        printf("[%s] Process ID #%i decrypted %s successfully. \n",CurrTime(ltime),getpid(),input);
        else
        printf("Decryption Error. Moving to next decryption.\n");    
        
        printf("child %i Setting status to ready \n", getpid());
        write(ProcessReadyPipe[i][1],&ProcessStatus,sizeof(ProcessStatus));
        }
        
        // finish reading everything, should send a ready status through
        
//        close(ProcessReadyPipe[i][1]);
        return;
        }
        else{
        ProcessCleanup(i, ProcessArr); 
        fclose(input);
        return;
        }
}
 
 
// children processes are instantiated

int dircount;


while(fgets(directory, 2050, input)){

// put them all into the processes available first.
if (dircount < ProcessTotal){
char* dir = strtok(directory, "\n");
directorysize = strlen(dir);
write(EncryptionDirectoryPipe[dircount][1], &directorysize, sizeof(int));
write(EncryptionDirectoryPipe[dircount][1], dir, strlen(dir));
dircount++;
}
// next, wait on select to see when the process is free
else {
int ret = select(FD_SETSIZE, &rfds, NULL,NULL,&tv);
if (ret>0){
for (int i = 0; i<= ProcessTotal;i++){
        if (FD_ISSET(ProcessReadyPipe[i][0], &rfds)){
        int statusbuffer;
        read(ProcessReadyPipe[i][0], statusbuffer,sizeof(int));
        char* dir = strtok(directory, "\n");
        directorysize = strlen(dir);
        write(EncryptionDirectoryPipe[i][1], &directorysize, sizeof(int));
        write(EncryptionDirectoryPipe[i][1], dir, strlen(dir));
        dircount++;

        // reset the FDset
        for (int i= 0; i < ProcessTotal; i++){
        FD_SET(ProcessReadyPipe[i][0],&rfds);}
        break;}
        }
}

}

}


for (int i = 0; i< ProcessTotal; i++){
close(EncryptionDirectoryPipe[i][1]);}
ProcessCleanup(ProcessesCreated, ProcessArr);
fclose(input);
exit(0);
}


// ProcessClean will take the dyn array ProcessArr and call a waitpid on every child, catching any errors.
void ProcessCleanup(int ProcessCount, int ProcessArr[]){
for (int i = 0; i < ProcessCount; i++){
        printf("Process cleaning \n");
        waitpid(ProcessArr[i], &pidstatus,NULL);
        // find a way to check if exit is wrong
        if ((!WIFEXITED(pidstatus)) || (WIFSIGNALED(pidstatus)) || (WEXITSTATUS(pidstatus)))
                printf("[%s] Child Process ID #%i did not terminate successfully. \n",CurrTime(ltime),ProcessArr[i]);}
}




// reset the FD set so we can run it again


















