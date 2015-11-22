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


Due to the amount of twitter messages needed to be decrypted, Lyrebird can now run its decryption with an optimized number of processes with your selection of process management algorithm. 
It will do this by first taking in the algorithm in the first line of the config file, then taking in a list text with directories of all the decrypted messages and its designated output directory. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/wait.h>
#include "memwatch.h"
#include <unistd.h>
#include <sys/types.h>
#include "utilfunc.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#define Want_Debug

time_t ltime;

void CreateChildProcesses();
void FCFSDecryption(char* job, int jobcount, int ProcessTotal, int * EDPipe[][2], int * PRPipe[][2], fd_set* rfds);


int main (int argc, char *argv[]) {
int listensocket;
int sockaddrlen = sizeof(struct sockaddr);
struct sockaddr_in serveraddress;
int readystatus = 1;
fd_set listenfd;
FD_ZERO(&listenfd);
int jobcount;




struct timeval tv;
tv.tv_sec = 2;
tv.tv_usec = 0; 


//has to be exactly 2 arguments
if (argc != 3){
	printf( "You have put too many/few arguments.\n");
	exit(-1);}

// set up socket 
listensocket = socket(AF_INET, SOCK_STREAM, 0);
FD_SET(listensocket,&listenfd);

// set server address to the user choices
serveraddress.sin_addr.s_addr = inet_addr(argv[1]);
serveraddress.sin_port = htons(atoi(argv[2]));

connect(listensocket, &serveraddress, sizeof(struct sockaddr_in));
printf("connected to server \n");
// create children for processing
CreateChildProcesses();

while(1){
// write that I have a child ready.
write(listensocket,&readystatus, sizeof(readystatus));
if(select(FD_SETSIZE, &listenfd, NULL,NULL,&tv)>0){
	int dirsize = 0;
	read(listensocket, &dirsize,sizeof(int));
	// check to see if socket was closed = jobs are done.
	if(dirsize == 0){
		//printf("return of read is 0, leaving while loop. \n");
		break;
	}
	printf("new message incoming from server \n");
	char dirbuffer[dirsize+1];
	bzero(&dirbuffer, dirsize+1);
	read(listensocket, dirbuffer, dirsize);
	printf("recieved message is :%s \n", dirbuffer);
	// give it to one of the children.
//	FCFSDecryption(dirbuffer, &jobcount, ProcessTotal, &EncryptionDirectoryPipe[ProcessTotal][2], &ProcessReadyPipe[ProcessTotal][2], &rfds);
	FD_SET(listensocket, &listenfd);
}
else FD_SET(listensocket, &listenfd);
}

printf("no more messages, exiting \n");
close(listensocket);
/*
// jobs are all given out, close off all your encryptiondirectorypipes and
//clean the rest up.
for (int i = 0; i< ProcessTotal; i++){
close(EncryptionDirectoryPipe[i][1]);}

ProcessCleanup(ProcessTotal, ProcessArr);
*/

exit(0);

}








void CreateChildProcesses(){


int ProcessCount = 0;
fd_set rfds;
fd_set wfds;
// we are only using the number of cores - because we restrict one to the parent
// from CreateChildProcesses
int CoreNumber = sysconf(_SC_NPROCESSORS_ONLN);
int ProcessTotal = CoreNumber-1;
int ProcessArr[ProcessTotal];
int EncryptionDirectoryPipe[ProcessTotal][2];
int ProcessReadyPipe[ProcessTotal][2];

FD_ZERO(&rfds);
FD_ZERO(&wfds);
// timeval is used for the SELECT function
struct timeval tv;
tv.tv_sec = 1;
tv.tv_usec = 0; 
 

int ProcessStatus = 1;
int RRcount = 0;
char* directory[2050];
int directorysize;
time_t ltime;



for(int i = 0; i < ProcessTotal; i++){
        // create the pipes. Include error handling if it fails.
        if(pipe(EncryptionDirectoryPipe[i])||pipe(ProcessReadyPipe[i]))
        {
            printf("[%s] Parent #%i failed to pipe. Exiting.\n",CurrTime(ltime), getpid());
            exit(-1);
        }
 
        int ChildID = fork();
        if (ChildID){           // parent process
        FD_SET(ProcessReadyPipe[i][1],&wfds);
        FD_SET(EncryptionDirectoryPipe[i][1],&wfds);
        FD_SET(ProcessReadyPipe[i][0],&rfds);
        // close the writing end of the pipe, we willl be reading from the child for its status
        close(ProcessReadyPipe[i][1]);
        // close reading end, we will be writing to the child processes
        close(EncryptionDirectoryPipe[i][0]);
        // add the child into the ProcessArr to keep track of processes created
        ProcessArr[i] = ChildID;
}
 
 
        else if (ChildID == 0){         // child process
        // close appropriate ends of the pipe for the child process
        close(ProcessReadyPipe[i][0]);
        close(EncryptionDirectoryPipe[i][1]);

        while(1){
        // read from the pipe the length of the incoming msg into dirsize. If its null, the pipe is empty, 
        // so finish the loop with a break.
        int dirsize = 0;
        printf("created child #%i for decryption\n", getpid());
        int readstatus = read(EncryptionDirectoryPipe[i][0], &dirsize,sizeof(int));
        if (readstatus==0){
                break;}
        // get the incoming msg using the size dirsize given in the privious msg
        // create character array with null termination, then zero it out
        char directorybuffer[dirsize+1];
        bzero(&directorybuffer, dirsize+1);
        read(EncryptionDirectoryPipe[i][0], directorybuffer,dirsize);
        // parse the directory into input and output pointers respectfully
        char *inouttemp = strtok(directorybuffer, " ");
        char *input = inouttemp;
        inouttemp = strtok(NULL, " ");
        char *output = inouttemp;
        output = strtok(output, "\n");
        // run decryption
        printf("[%s] Child Process ID #%i will decrypt %s. \n",CurrTime(ltime),getpid(),input);
        if(decrypt(input,output) > 0)
        printf("[%s] Process ID #%i decrypted %s successfully. \n",CurrTime(ltime),getpid(),input);
        // Case for FCFS: send msg to parent saying its ready.
        write(ProcessReadyPipe[i][1],&ProcessStatus,sizeof(ProcessStatus));
        }
        // Process is done all jobs. Close its writing pipe.
        printf("closing all work, jobs are done \n");
        close(ProcessReadyPipe[i][1]);
        exit(0);
        }
        else{
        // Parent fails to fork. Clean up previously created children prior to crash
        printf("[%s] Process ID #%i failed to create a child. \n",CurrTime(ltime), getpid());
        ProcessCleanup(i, ProcessArr); 
       // fclose(input);
        exit(-1);
        }
}
}



void FCFSDecryption(char* job, int jobcount, int ProcessTotal, int * EDPipe[][2], int * PRPipe[][2], fd_set* rfds){

int directorysize;
// timeval is used for the SELECT function
struct timeval tv;
tv.tv_sec = 1;
tv.tv_usec = 0; 

// direct all the children with a first set of work first
if (jobcount < ProcessTotal){
char* dir = strtok(job, "\n");
directorysize = strlen(dir);
write(EDPipe[jobcount][1], &directorysize, sizeof(int));
write(EDPipe[jobcount][1], dir, strlen(dir));
jobcount++;
}
// next, wait on select to see when the process is free using SELECT, then 
//check to see which process is ready, then send the next job to it.
else {
int ret = select(FD_SETSIZE, &rfds, NULL,NULL,&tv);
if (ret>0){
for (int i = 0; i<= ProcessTotal;i++){
        if (FD_ISSET(PRPipe[i][0], &rfds)){
        int statusbuffer;
        read(PRPipe[i][0], statusbuffer,sizeof(int));
        char* dir = strtok(job, "\n");
        directorysize = strlen(dir);
        write(EDPipe[i][1], &directorysize, sizeof(int));
        write(EDPipe[i][1], dir, strlen(dir));
        jobcount++;

        // reset the FDset
        for (int i= 0; i < ProcessTotal; i++){
        FD_SET(PRPipe[i][0],rfds);} 
        break;}
        }
}

}




}






